/***************************************************************************
 *   Copyright (C) 2004-2006 by Thiago Silva                               *
 *   thiago.silva@kdemail.net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "editorui.h"
#include "document.h"

#include "mainwindow.h"
#include "statusbarwidget.h"
#include "protoeditor.h"

#include <kparts/partmanager.h>
#include <ktexteditor/popupmenuinterface.h>
#include <ktexteditor/view.h>
#include <kactioncollection.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <kpopupmenu.h>
#include <kfileitem.h>
#include <kurldrag.h>
#include <qdragobject.h> 
#include <qdir.h>
#include <klocale.h>

EditorUI::EditorUI(QWidget* parent, MainWindow* mwindow, const char *name)
    : KTabWidget(parent, name), m_terminating(false), m_currentView(0)
{
  //Tabs control
  (void)new KAction(i18n("Activate Next Tab"), "tab_next", 
    "Alt+Right", this, SLOT(slotActivateNextTab()), 
      mwindow->actionCollection(), "activatenexttab" );

  (void)new KAction(i18n("Activate Previous Tab"), "tab_previous", 
    "Alt+Left", this, SLOT(slotActivatePrevTab()), mwindow->actionCollection(), "activateprevtab" );

  (void)new KAction(i18n("Raise Editor"), "tab_next", 
    "Alt+C", this, SLOT(slotFocusCurrentDocument()), mwindow->actionCollection(), "focuscurrentdocument" );


  (void)new KAction(i18n("Add Watch"), "math_brace", 0, this,
                    SLOT(slotAddWatch()), mwindow->actionCollection(), "editor_add_watch");

  (void)new KAction(i18n("Configure &Editor..."), 0, 0, 
    this, SLOT(slotConfigureEditor()), mwindow->actionCollection(), "settings_editor");

  setAcceptDrops(TRUE);
  
  connect(this, SIGNAL(currentChanged(QWidget*)), this, SLOT(slotCurrentChanged(QWidget*)));

  //reordering with middle button: need extra coding to work (it alters the index of the tabs
  //wich should be in sync with the list)
  //   setTabReorderingEnabled(true);
  setHoverCloseButton (true);
  setHoverCloseButtonDelayed (true);

  KConfig* config = KGlobal::config();
  
  if(!config->hasGroup(QString("Kate View Defaults"))) {
    config->setGroup("Kate View Defaults");
    config->writeEntry("Icon Bar","true", true);
    config->writeEntry("Line Numbers","true",true);
  }
}

EditorUI::~EditorUI()
{
  terminate();
}

void EditorUI::terminate()
{
  m_terminating = true;
  closeAllDocuments();
}

void EditorUI::createNew() {
  createDocument();
}

bool EditorUI::openDocument(const KURL& url)
{
  int index;
  if((index = documentIndex(url)) != -1)
  {
    //file already opened, show it
    this->setCurrentPage(index);
    return true;
  }
  else
  {
    if(createDocument(url))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

bool EditorUI::closeDocument(int index)
{
  if((index < 0) || (count() == 0))
  {
    return false;
  }

  Document* doc = document(index);

  if(!doc->close()) return false; //file is modifiyed and the user hit "cancel" on prompt to save

  if(!m_terminating)
  {
    Protoeditor::self()->mainWindow()->guiFactory()->removeClient(doc->view());
    if(index == currentPageIndex())
    {
      m_currentView = 0L;
    }
  }

  QWidget* w = page(index);
  removePage(w);

  m_docList.remove(doc);
  delete doc;

  if((count() == 0) && !m_terminating)
  {
    emit sigEmpty();
  }

  return true;
}

bool EditorUI::closeCurrentDocument()
{
  return closeDocument(currentPageIndex());
}

bool EditorUI::closeAllDocuments()
{
  while(count())
  {
    //stop closing documents if the user hit "cancel" on save prompt
    if(!closeCurrentDocument()) return false;
  }

  return true;
}

void EditorUI::setCurrentDocumentTab(const KURL& url, bool forceOpen)
{
  int index = documentIndex(url);

  /* currentPageIndex() == index:
      setCurrentDocumentTab() is activated  when debug starts.
      setCurrentPage() emits currentChanged() -> slotCurrentChanged().
      In slotCurrentChanged(), action state changes to has_fileopen
      and this ativate buttons like "debug" and "run".
      For any effects, this avoids, at least, set the currentPage twice
  */
  if((m_docList.count() > 0) && (currentPageIndex() == index)) return;

  if(index != -1 ) 
  {
    setCurrentPage(index);
  }
  else if(forceOpen)
  {
    createDocument(url);
  }
}

bool EditorUI::saveCurrentFile()
{
  if(count() == 0) return false;

  return currentDocument()->save();
}

bool EditorUI::saveCurrentFileAs(const KURL& url, const QString& encoding)
{
  if(!encoding.isEmpty()) {
    currentDocument()->setEncoding(encoding);
  }

  if(count() == 0) return false;

  bool ret = currentDocument()->saveAs(url);
  return ret;
}

void EditorUI::saveExistingFiles()
{
  QValueList<Document*>::iterator it;
  for(it = m_docList.begin(); it != m_docList.end(); ++it)
  {
    if((*it)->existsOnDisk() && (*it)->isModified())
    {
      (*it)->save();
    }
  }    
}

void EditorUI::gotoLineAtFile(const KURL& url, int line)
{
  setCurrentDocumentTab(url, true);

  if(currentPageIndex() == -1) return;

  currentDocument()->gotoLine(line);
}


KURL EditorUI::documentURL(int index)
{
  int size = m_docList.count();
  if((index >= 0) && (index < size))
  {
    return document(index)->url();
  }
  else
  {
    return KURL();
  }
}

KURL EditorUI::currentDocumentURL()
{
  return documentURL(currentPageIndex());
}

int EditorUI::currentDocumentLine()
{
  if(currentPageIndex() == -1) return 0;

  return currentDocument()->currentLine();
}

bool EditorUI::currentDocumentExistsOnDisk()
{
  return currentDocument()->existsOnDisk();
}

void EditorUI::markActiveBreakpoint(const KURL& url, int line)
{
  Document* doc = document(url);
  if(doc)
  {
    document(url)->markActiveBreakpoint(line);
  }
}

void EditorUI::unmarkActiveBreakpoint(const KURL& url, int line)
{
  Document* doc = document(url);
  if(doc)
  {
    document(url)->unmarkActiveBreakpoint(line);
  }
}

void EditorUI::markDisabledBreakpoint(const KURL& url, int line)
{
  Document* doc = document(url);
  if(doc)
  {
    document(url)->markDisabledBreakpoint(line);
  }
}

void EditorUI::unmarkDisabledBreakpoint(const KURL& url, int line)
{
  Document* doc = document(url);
  if(doc)
  {
    document(url)->unmarkDisabledBreakpoint(line);
  }
}

void EditorUI::markExecutionPoint(const KURL& url, int line)
{
  Document* doc = document(url);
  if(doc)
  {
    document(url)->markExecutionPoint(line);
  }
}

void EditorUI::unmarkExecutionPoint(const KURL& url)
{
  Document* doc = document(url);
  if(doc)
  {
    document(url)->unmarkExecutionPoint();
  }
}

void EditorUI::markPreExecutionPoint(const KURL& url, int line)
{
  Document* doc = document(url);
  if(doc)
  {
    document(url)->markPreExecutionPoint(line);
  }
}

void EditorUI::unmarkPreExecutionPoint(const KURL& url)
{
  Document* doc = document(url);
  if(doc)
  {
    document(url)->unmarkPreExecutionPoint();
  }
}


bool EditorUI::hasBreakpointAt(const KURL& url, int line)
{
  Document* doc = document(url);
  if(doc)
  {
    return document(url)->hasBreakpointAt(line);
  }
  else
  {
    return false;
  }
}

void EditorUI::createDocument() {
  QString home = QDir::homeDirPath();
  KURL url = KURL::fromPathOrURL(i18n("%1/untitled_%2").arg(home, QString::number(count()+1)));
  Document* doc = new Document(this, url);

  initDoc(doc);

  doc->view()->setFocus();
}

bool EditorUI::createDocument(const KURL& url) {
  Document* doc = new Document(this);

  if(!doc->open(url))
  {
    delete doc;
    return false;
  }
  initDoc(doc);
  return true;
}

void EditorUI::initDoc(Document* doc)
{
  connect(doc, SIGNAL(sigBreakpointMarked(Document*, int, bool )), this,
          SLOT(slotBreakpointMarked(Document*, int, bool)));

  connect(doc, SIGNAL(sigBreakpointUnmarked(Document*, int )), this,
          SLOT(slotBreakpointUnmarked(Document*, int)));

  connect(doc, SIGNAL(sigTextChanged()), this,
          SLOT(slotTextChanged()));

  connect(doc, SIGNAL(sigStatusMsg(const QString&)), this,
          SLOT(slotStatusMsg(const QString&)));

  connect(doc, SIGNAL(sigDocumentSaved(Document*)), this, 
          SLOT(slotDocumentSaved(Document*)));  

  connect(doc->view(), SIGNAL(dropEventPass(QDropEvent*)),
    this, SLOT(slotDropEvent(QDropEvent*)));

  QIconSet mimeIcon(KMimeType::pixmapForURL(doc->url(), 0, KIcon::Small));
  if (mimeIcon.isNull())
  {
    mimeIcon = QIconSet(SmallIcon("document"));
  }

  addTab(doc->tab(), mimeIcon, doc->url().fileName());
  setTabToolTip(doc->tab(), doc->url().prettyURL());

  KTextEditor::PopupMenuInterface* popupIf = dynamic_cast<KTextEditor::PopupMenuInterface*>(doc->view());
  if (popupIf)
  {
    QPopupMenu *popup = (QPopupMenu*) 
      Protoeditor::self()->mainWindow()->factory()->container(
        "ktexteditor_popup", Protoeditor::self()->mainWindow());

    popupIf->installPopup(popup);
    connect(popup, SIGNAL(aboutToShow()), this, SLOT(slotMenuAboutToShow()));
  }

  m_docList.append(doc); 
  
  setCurrentPage(count()-1);

  if(count() == 1) 
  {
    emit sigFirstPage();
  }

  emit sigNewPage();
}

int EditorUI::documentIndex(const KURL& url)
{
  int i = 0;

  QValueList<Document*>::iterator it;
  for (it = m_docList.begin(); it != m_docList.end(); ++it, i++ )
  {
    if((*it)->url() == url)
    {
      return i;
    }
  }
  return -1;
}

void EditorUI::closeRequest(int index)
{
  closeDocument(index);
}

void EditorUI::slotCurrentChanged(QWidget*)
{
  if(m_terminating) return;

  Protoeditor::self()->mainWindow()->setCaption(currentDocumentURL().prettyURL());

  if(m_currentView)
  {
    Protoeditor::self()->mainWindow()->guiFactory()->removeClient(m_currentView);
  }

  if(count())
  {    
    m_currentView = document(currentPageIndex())->view();
    Protoeditor::self()->mainWindow()->guiFactory()->addClient(m_currentView);
  }
}

void EditorUI::slotBreakpointMarked(Document* doc, int line, bool enabled)
{
  emit sigBreakpointMarked(doc->url(), line, enabled);
}

void EditorUI::slotBreakpointUnmarked(Document* doc, int line)
{
  emit sigBreakpointUnmarked(doc->url(), line);
}

void EditorUI::slotTextChanged()
{
  if(currentPageIndex() == -1) return;

  int index = currentPageIndex();
  Document* doc = document(index);

  if(doc->isModified())
  {
    setTabIconSet(page(index), QIconSet(SmallIcon("filesave")));
  }
  else
  {
    QIconSet mimeIcon(KMimeType::pixmapForURL(doc->url(), 0, KIcon::Small));
    if (mimeIcon.isNull())
    {
      mimeIcon = QIconSet(SmallIcon("document"));
    }
    setTabIconSet(page(index), mimeIcon);
  }
}

void EditorUI::slotStatusMsg(const QString& msg)
{
  Protoeditor::self()->mainWindow()->statusBar()->setEditorStatus(msg);
}

void EditorUI::slotDocumentSaved(Document* doc)
{
    QIconSet mimeIcon(KMimeType::pixmapForURL(doc->url(), 0, KIcon::Small));
    if (mimeIcon.isNull())
    {
      mimeIcon = QIconSet(SmallIcon("document"));
    }
    setTabLabel(doc->tab(), doc->url().fileName());
    setTabIconSet(doc->tab(), mimeIcon);
    setTabToolTip(doc->tab(), doc->url().prettyURL());  
}

void EditorUI::contextMenu(int index, const QPoint & p)
{
  enum { Close, CloseOthers, CloseAll };

  KPopupMenu* menu = new KPopupMenu(this);
  menu->insertItem(SmallIcon("fileclose"), i18n("Close"), Close);
  menu->insertItem(i18n("Close Other Tabs"), CloseOthers);
  menu->insertItem(i18n("Close All"), CloseAll);

  Document* doc = document(index);
  int idx = 0;
  switch(menu->exec(p))
  {
    case Close:
      closeDocument(index);
      break;
    case CloseOthers:
      while(count() != 1)
      {
        if(document(idx) != doc)
        {
          //stop closing documents if the user hits "cancel" on save dialog
          if(!closeDocument(idx)) break;
        }
        else
        {
          idx++;
        }
      }
      break;
    case CloseAll:
      closeAllDocuments();
      break;
  }

  delete menu;
}

KTextEditor::View* EditorUI::currentView()
{
  if(count() == 0) return 0;
  return m_currentView;
}

Document* EditorUI::document(unsigned int index)
{
  if(index > (m_docList.count()-1))
  {
    return 0L;
  }
  else
  {
    return *(m_docList.at(index));
  }
}

Document* EditorUI::document(const KURL& url)
{
  QValueList<Document*>::iterator it;
  for(it = m_docList.begin(); it != m_docList.end(); ++it)
  {

    if((*it)->url() == url)
    {
      return (*it);
    }
  }

  return 0L;
}

Document* EditorUI::currentDocument()
{
  return document(currentPageIndex());
}

void EditorUI::slotMenuAboutToShow()
{
  KAction* ac = Protoeditor::self()->mainWindow()->actionCollection()->action("editor_add_watch");
  if(ac)
  {
    QString watch = currentDocument()->wordUnderCursor();
    if(watch.isEmpty())
    {
      ac->setText(i18n("Add Watch"));
      ac->setEnabled(false);
    }
    else
    {
      ac->setEnabled(true);
      ac->setText(i18n("Add Watch: ") + watch);
    }
  }
}

void EditorUI::slotAddWatch()
{
  emit sigAddWatch(currentDocument()->wordUnderCursor());
}

void EditorUI::dragEnterEvent(QDragEnterEvent* e)
{
  if(KURLDrag::canDecode(e)) {
    e->accept(rect());
  }
}

void EditorUI::dragMoveEvent( QDragMoveEvent *e )
{
   e->accept();
}
    
void EditorUI::dropEvent(QDropEvent* e)
{
  slotDropEvent(e);
}

void EditorUI::slotDropEvent(QDropEvent* e) {
  KURL::List list;
  if(KURLDrag::decode(e, list)) {
    int size  = list.size();
    for(int i = 0; i < size; i++)
    {
      Protoeditor::self()->openFile(list[i]);
    }
  }  
}

void EditorUI::slotActivateNextTab()
{
  if(m_docList.count() == 0) return;

  unsigned int index = currentPageIndex() + 1;
  if(index < m_docList.count())
  {
    setCurrentPage(index);
  }  
}

void EditorUI::slotActivatePrevTab()
{
  if(m_docList.count() == 0) return;

  unsigned int index = currentPageIndex() - 1;
  if(index < m_docList.count())
  {
    setCurrentPage(index);
  }  
}

void EditorUI::slotFocusCurrentDocument()
{
  if(m_currentView) m_currentView->setFocus();
}

void EditorUI::slotConfigureEditor()
{
  Document::configureEditor(currentView());
}

#include "editorui.moc"
