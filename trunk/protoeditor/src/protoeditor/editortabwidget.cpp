/***************************************************************************
 *   Copyright (C) 2004 by Thiago Silva                                    *
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

#include "editortabwidget.h"
#include "mainwindow.h"
#include "document.h"

#include <kparts/partmanager.h>
#include <ktexteditor/popupmenuinterface.h>
#include <ktexteditor/view.h>
#include <kactioncollection.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <kpopupmenu.h>

EditorTabWidget::EditorTabWidget(QWidget* parent, MainWindow *window, const char *name)
    : KTabWidget(parent, name), m_terminating(false),
    m_window(window), m_currentView(0)
{
  connect(this, SIGNAL(currentChanged(QWidget*)), this, SLOT(slotCurrentChanged(QWidget*)));

  //reordering with middle button: need extra coding to work (it alters the index of the tabs
  //wich should be in sync with the list)
  //   setTabReorderingEnabled(true);
  setHoverCloseButton (true);
  setHoverCloseButtonDelayed (true);
}

EditorTabWidget::~EditorTabWidget()
{
  terminate();
}

void EditorTabWidget::terminate()
{
  m_terminating = true;
  closeAllDocuments();
}


bool EditorTabWidget::openDocument(const KURL& url)
{
  int index;
  if((index = documentIndex(url.path())) != -1)
  {
    //file already opened, show it
    this->setCurrentPage(index);
    return true;
  }
  else
  {
    if(createDocument(url))
    {
      emit sigNewDocument();
      return true;
    }
    else
    {
      return false;
    }
  }
}

bool EditorTabWidget::closeDocument(int index)
{
  if((index < 0) || (count() == 0))
  {
    return false;
  }

  Document* doc = document(index);

  if(!doc->close()) return false; //file is modifiyed and the user hit "cancel" on prompt to save

  if(!m_terminating)
  {
    m_window->guiFactory()->removeClient(doc->view());
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
    m_window->actionStateChanged("has_nofileopened");
    m_window->setCaption(QString::null);
    m_window->setEditorStatusMsg(QString::null);
  }

  return true;
}

bool EditorTabWidget::closeCurrentDocument()
{
  return closeDocument(currentPageIndex());
}

bool EditorTabWidget::closeAllDocuments()
{
  while(count())
  {
    //stop closing documents if the user hit "cancel" on save prompt
    if(!closeCurrentDocument()) return false;
  }

  return true;
}

void EditorTabWidget::setCurrentDocumentTab(const QString& filePath, bool forceOpen)
{
  int index = documentIndex(filePath);
  if(index != -1)
  {
    setCurrentPage(index);
  }
  else if(forceOpen)
  {
    createDocument(KURL(filePath));
  }
}

bool EditorTabWidget::saveCurrentFile()
{
  if(count() == 0) return false;

  return currentDocument()->save();
}

bool EditorTabWidget::saveCurrentFileAs(const KURL & url)
{
  if(count() == 0) return false;

  return currentDocument()->saveAs(url);
}

void EditorTabWidget::gotoLineAtFile(const QString& filePath, int line)
{
  setCurrentDocumentTab(filePath, true);

  if(currentPageIndex() == -1) return;

  currentDocument()->gotoLine(line);
}


const QString& EditorTabWidget::documentPath(int index)
{
  int size = m_docList.count();
  if((index >= 0) && (index < size))
  {
    return document(index)->path();
  }
  else
  {
    return QString::null;
  }
}

const QString& EditorTabWidget::currentDocumentPath()
{
  return documentPath(currentPageIndex());
}

int EditorTabWidget::currentDocumentLine()
{
  if(currentPageIndex() == -1) return 0;

  return currentDocument()->currentLine();
}

void EditorTabWidget::markActiveBreakpoint(const QString& filePath, int line)
{
  Document* doc = document(filePath);
  if(doc)
  {
    document(filePath)->markActiveBreakpoint(line);
  }
}

void EditorTabWidget::unmarkActiveBreakpoint(const QString& filePath, int line)
{
  Document* doc = document(filePath);
  if(doc)
  {
    document(filePath)->unmarkActiveBreakpoint(line);
  }
}

void EditorTabWidget::markDisabledBreakpoint(const QString& filePath, int line)
{
  Document* doc = document(filePath);
  if(doc)
  {
    document(filePath)->markDisabledBreakpoint(line);
  }
}

void EditorTabWidget::unmarkDisabledBreakpoint(const QString& filePath, int line)
{
  Document* doc = document(filePath);
  if(doc)
  {
    document(filePath)->unmarkDisabledBreakpoint(line);
  }
}

void EditorTabWidget::markExecutionPoint(const QString& filePath, int line)
{
  Document* doc = document(filePath);
  if(doc)
  {
    document(filePath)->markExecutionPoint(line);
  }
}

void EditorTabWidget::unmarkExecutionPoint(const QString& filePath)
{
  Document* doc = document(filePath);
  if(doc)
  {
    document(filePath)->unmarkExecutionPoint();
  }
}

void EditorTabWidget::markPreExecutionPoint(const QString& filePath, int line)
{
  Document* doc = document(filePath);
  if(doc)
  {
    document(filePath)->markPreExecutionPoint(line);
  }
}

void EditorTabWidget::unmarkPreExecutionPoint(const QString& filePath)
{
  Document* doc = document(filePath);
  if(doc)
  {
    document(filePath)->unmarkPreExecutionPoint();
  }
}

bool EditorTabWidget::hasBreakpointAt(const QString& filePath, int line)
{
  Document* doc = document(filePath);
  if(doc)
  {
    return document(filePath)->hasBreakpointAt(line);
  }
  else
  {
    return false;
  }
}

bool EditorTabWidget::createDocument(const KURL& url)
{
  Document* doc = new Document(this);

  if(!doc->open(url))
  {
    delete doc;
    return false;
  }

  connect(doc, SIGNAL(sigBreakpointMarked(Document*, int )), this,
          SLOT(slotBreakpointMarked(Document*, int)));

  connect(doc, SIGNAL(sigBreakpointUnmarked(Document*, int )), this,
          SLOT(slotBreakpointUnmarked(Document*, int)));

  connect(doc, SIGNAL(sigTextChanged()), this,
          SLOT(slotTextChanged()));

  connect(doc, SIGNAL(sigStatusMsg(const QString&)), this,
          SLOT(slotStatusMsg(const QString&)));

  QIconSet mimeIcon (KMimeType::pixmapForURL(doc->path(), 0, KIcon::Small));
  if (mimeIcon.isNull())
  {
    mimeIcon = QIconSet(SmallIcon("document"));
  }

  addTab(doc->tab(), mimeIcon, url.fileName());
  setTabToolTip(doc->tab(), doc->path());

  KTextEditor::PopupMenuInterface* popupIf = dynamic_cast<KTextEditor::PopupMenuInterface*>(doc->view());
  if (popupIf)
  {
    QPopupMenu *popup = (QPopupMenu*) m_window->factory()->container("ktexteditor_popup", m_window);
    popupIf->installPopup(popup);
    connect(popup, SIGNAL(aboutToShow()), this, SLOT(slotMenuAboutToShow()));
  }

  m_docList.append(doc);

  setCurrentPage(count()-1);
  return true;
}

int EditorTabWidget::documentIndex(const QString& filePath)
{
  int i = 0;

  QValueList<Document*>::iterator it;
  for (it = m_docList.begin(); it != m_docList.end(); ++it, i++ )
  {
    if((*it)->path() == filePath)
    {
      return i;
    }
  }
  return -1;
}

void EditorTabWidget::closeRequest(int index)
{
  closeDocument(index);
}

void EditorTabWidget::slotCurrentChanged(QWidget*)
{
  if(m_terminating) return;

  m_window->setCaption(currentDocumentPath());

  if(m_currentView)
  {
    m_window->guiFactory()->removeClient(m_currentView);
  }

  if(count())
  {
    m_window->actionStateChanged("has_fileopened");

    m_currentView = document(currentPageIndex())->view();
    m_window->guiFactory()->addClient(m_currentView);
  }
}

void EditorTabWidget::slotBreakpointMarked(Document* doc, int line)
{
  emit sigBreakpointMarked(doc->path(), line);
}

void EditorTabWidget::slotBreakpointUnmarked(Document* doc, int line)
{
  emit sigBreakpointUnmarked(doc->path(), line);
}

void EditorTabWidget::slotTextChanged()
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
    QIconSet mimeIcon(KMimeType::pixmapForURL(doc->path(), 0, KIcon::Small));
    if (mimeIcon.isNull())
    {
      mimeIcon = QIconSet(SmallIcon("document"));
    }
    setTabIconSet(page(index), mimeIcon);
  }
}

void EditorTabWidget::slotStatusMsg(const QString& msg)
{
  m_window->setEditorStatusMsg(msg);
}

void EditorTabWidget::contextMenu(int index, const QPoint & p)
{
  enum { Close, CloseOthers, CloseAll };

  KPopupMenu* menu = new KPopupMenu(this);
  menu->insertItem(SmallIcon("fileclose"), "Close", Close);
  menu->insertItem("Close Other Tabs", CloseOthers);
  menu->insertItem("Close All", CloseAll);

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

KTextEditor::View* EditorTabWidget::currentView()
{
  if(count() == 0) return 0;
  return m_currentView;
}

Document* EditorTabWidget::document(uint index)
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

Document* EditorTabWidget::document(const QString& filePath)
{
  QValueList<Document*>::iterator it;
  for(it = m_docList.begin(); it != m_docList.end(); ++it)
  {

    if((*it)->path() == filePath)
    {
      return (*it);
    }
  }

  return 0L;
}

Document* EditorTabWidget::currentDocument()
{
  return document(currentPageIndex());
}

void EditorTabWidget::slotMenuAboutToShow()
{
  KAction* ac = m_window->actionCollection()->action("editor_add_watch");
  if(ac)
  {
    QString watch = currentDocument()->wordUnderCursor();
    if(watch.isEmpty())
    {
      ac->setText(QString("Add Watch"));
      ac->setEnabled(false);
    }
    else
    {
      ac->setEnabled(true);
      ac->setText(QString("Add Watch: ") + watch);
    }
  }
}

void EditorTabWidget::slotAddWatch()
{
  emit sigAddWatch(currentDocument()->wordUnderCursor());
}

#include "editortabwidget.moc"
