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

#include <ktexteditor/document.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/view.h>
#include <ktexteditor/markinterface.h>
#include <ktexteditor/markinterfaceextension.h>
#include <ktexteditor/configinterface.h>
#include <ktexteditor/viewcursorinterface.h>
#include <ktexteditor/clipboardinterface.h>
#include <ktexteditor/selectioninterface.h>
#include <ktexteditor/undointerface.h>
#include <kparts/partmanager.h>
#include <ktexteditor/popupmenuinterface.h>
#include <kactioncollection.h>
#include <kiconloader.h>
#include <kdeversion.h>

#include <qlayout.h>

EditorTabWidget::EditorTabWidget(QWidget* parent, MainWindow *window, const char *name)
    : KTabWidget(parent, name), m_terminating(false), m_markGuard(false),m_closeGuard(false),
    m_window(window), m_currentView(0)
{
//   m_partManager = new KParts::PartManager(m_window);

  connect(this, SIGNAL(currentChanged(QWidget*)), this, SLOT(slotCurrentChanged(QWidget*)));
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


void EditorTabWidget::openDocument(KURL url)
{
  int index;
  if((index = documentIndex(url.path())) != -1)
  {
    //file already opened, show it
    this->setCurrentPage(index);
  }
  else
  {
    createDocument(url);
    emit sigNewDocument();
  }
}

void EditorTabWidget::closeCurrentDocument()
{
  if(count() == 0) return;

  m_closeGuard = true;

  int index = currentPageIndex();

  QValueList<Document_t>::iterator it = m_docList.at(index);

  KTextEditor::Document* doc = (*it).view->document();
  doc->closeURL();

  KTextEditor::View* view = (*it).view;

  QWidget* w = page(index);
  removePage(w);
    
  delete view;
  delete doc;


  m_docList.remove(it);


  m_closeGuard = false;
}

void EditorTabWidget::closeAllDocuments()
{
  while(count())
  {
    closeCurrentDocument();
  }
  int size = m_docList.count();
  int x;
  x = 2;
  /*
  int i = 0;
  QWidget* w;
  KTextEditor::Document* doc;
  KTextEditor::View* view;

  QValueList<Document_t>::iterator it;
  for( it = m_docList.begin(); it != m_docList.end(); ++it, ++i )
  {
    view = (*it).view;
    doc = (*it).view->document();
    doc->closeURL();

    delete view;
    delete doc;

    w = page(i);
    removePage(w);

    i++;
  }

  m_docList.clear();
  */
}

void EditorTabWidget::setCurrentDocument(const QString& filePath, bool forceOpen)
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

  KTextEditor::Document *doc = (*(m_docList.at(currentPageIndex()))).view->document();
  return doc->save();
}

bool EditorTabWidget::saveCurrentFileAs(const KURL & url)
{
  if(count() == 0) return false;

  KTextEditor::Document *doc = (*(m_docList.at(currentPageIndex()))).view->document();
  return doc->saveAs(url);
}

void EditorTabWidget::gotoLineAtFile(const QString& filePath, int line)
{
  int index;
  KTextEditor::View* view;
  KTextEditor::ViewCursorInterface *vci;

  setCurrentDocument(filePath, true);

  if((index = currentPageIndex()) == -1) return;

  Document_t d;
  d = *(m_docList.at(index));
  view = d.view;

  vci = dynamic_cast<KTextEditor::ViewCursorInterface*>(view);
  vci->setCursorPosition(line, 0);
}


const QString& EditorTabWidget::documentPath(int index)
{
  int size = m_docList.count();
  if((index >= 0) && (index < size))
  {
    return (*(m_docList.at(index))).path;
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
  int index;
  uint line, col;
  KTextEditor::View* view;
  KTextEditor::ViewCursorInterface *vci;

  if((index = currentPageIndex()) == -1) return 0;

  Document_t d;
  d = *(m_docList.at(index));
  view = d.view;

  vci = dynamic_cast<KTextEditor::ViewCursorInterface*>(view);
  vci->cursorPosition(&line, &col);
  return line+1;
}


void EditorTabWidget::markActiveBreakpoint(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
  if(!imark) return;
  m_markGuard = true;
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imark->addMark(line-1, KTextEditor::MarkInterface::BreakpointActive);
#else
  imark->addMark(line-1, KTextEditor::MarkInterface::markType02);
#endif
  m_markGuard = false;
}

void EditorTabWidget::unmarkActiveBreakpoint(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
  m_markGuard = true;
  if(!imark) return;
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imark->removeMark(line-1, KTextEditor::MarkInterface::BreakpointActive);
#else
  imark->removeMark(line-1, KTextEditor::MarkInterface::markType02);
#endif
  m_markGuard = false;
}

void EditorTabWidget::markDisabledBreakpoint(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
  m_markGuard = true;
  if(!imark) return;
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imark->addMark(line-1, KTextEditor::MarkInterface::BreakpointDisabled);
#else
  imark->addMark(line-1, KTextEditor::MarkInterface::markType04);
#endif
  m_markGuard = false;
}

void EditorTabWidget::unmarkDisabledBreakpoint(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
  m_markGuard = true;
  if(!imark) return;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imark->removeMark(line-1, KTextEditor::MarkInterface::BreakpointDisabled);
#else
  imark->removeMark(line-1, KTextEditor::MarkInterface::markType04);
#endif
  m_markGuard = false;
}

void EditorTabWidget::markExecutionPoint(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
  if(!imark) return;
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imark->addMark(line-1, KTextEditor::MarkInterface::Execution);
#else
  imark->addMark(line-1, KTextEditor::MarkInterface::markType05);
#endif
}

void EditorTabWidget::unmarkExecutionPoint(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
  if(!imark) return;
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imark->removeMark(line-1, KTextEditor::MarkInterface::Execution);
#else
  imark->removeMark(line-1, KTextEditor::MarkInterface::markType05);
#endif
}

void EditorTabWidget::markPreExecutionPoint(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
  if(!imark) return;
  imark->addMark(line-1, KTextEditor::MarkInterface::markType08);
}

void EditorTabWidget::unmarkPreExecutionPoint(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
  if(!imark) return;
  imark->removeMark(line-1, KTextEditor::MarkInterface::markType08);
}

bool EditorTabWidget::hasBreakpointAt(const QString& filePath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filePath);
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  return
    imark->mark(line-1) &
    (KTextEditor::MarkInterface::BreakpointDisabled |
     KTextEditor::MarkInterface::BreakpointActive);
#else
  return
    imark->mark(line-1) &
    (KTextEditor::MarkInterface::markType04 |
     KTextEditor::MarkInterface::markType02);
#endif
}

void EditorTabWidget::createDocument(KURL url)
{

  KTextEditor::Document *doc =
    KTextEditor::EditorChooser::createDocument(
      0L, "KTextEditor::Document");

  if(!doc->openURL(url))
  {
    return;
  }

  QWidget* tab = new QWidget(this);
  QVBoxLayout *lay = new QVBoxLayout(tab, 1, 1);

  KTextEditor::View * view = doc->createView(tab);

  lay->addWidget(view);
  insertTab(tab, url.fileName());

  setupMarks(view);

  KTextEditor::PopupMenuInterface* popupIf = dynamic_cast<KTextEditor::PopupMenuInterface*>(view);
  if (popupIf)
    popupIf->installPopup((QPopupMenu *)m_window->factory()->container("ktexteditor_popup", m_window));

  Document_t d;
  d.path = url.path();
  d.view = view;
  m_docList.append(d);

  setCurrentPage(count()-1);
}

void EditorTabWidget::setupMarks(KTextEditor::View* view)
{
  KTextEditor::MarkInterfaceExtension* imarkex =
    dynamic_cast<KTextEditor::MarkInterfaceExtension*>(view->document());

  if(!imarkex)
  {
    return;
  }
  KIconLoader *loader = KGlobal::iconLoader();

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setPixmap(KTextEditor::MarkInterface::Execution, loader->loadIcon(
                       "executionpoint", KIcon::Small));
#else
  imarkex->setPixmap(KTextEditor::MarkInterface::markType05, loader->loadIcon(
                       "executionpoint", KIcon::Small));
#endif

  imarkex->setPixmap(KTextEditor::MarkInterface::markType08, loader->loadIcon(
                       "preexecutionpoint", KIcon::Small));

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setPixmap(KTextEditor::MarkInterface::BreakpointActive, loader->loadIcon(
                       "activebreakpoint",KIcon::Small));
#else
  imarkex->setPixmap(KTextEditor::MarkInterface::markType02, loader->loadIcon(
                       "activebreakpoint", KIcon::Small));
#endif

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setPixmap(KTextEditor::MarkInterface::BreakpointDisabled, loader->loadIcon(
                       "disabledbreakpoint",KIcon::Small));
#else
  imarkex->setPixmap(KTextEditor::MarkInterface::markType04, loader->loadIcon(
                       "disabledbreakpoint", KIcon::Small));
#endif

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setDescription(KTextEditor::MarkInterface::BreakpointActive, "Breakpoint");
#else
  imarkex->setDescription(KTextEditor::MarkInterface::markType02, "Breakpoint");
#endif

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setMarksUserChangable(KTextEditor::MarkInterface::Bookmark + KTextEditor::MarkInterface::BreakpointActive);
#else
  imarkex->setMarksUserChangable(KTextEditor::MarkInterface::markType01 + KTextEditor::MarkInterface::markType02);
#endif

}

int EditorTabWidget::documentIndex(const QString& filePath)
{
  int i = 0;

  QValueList<Document_t>::iterator it;
  for (it = m_docList.begin(); it != m_docList.end(); ++it, i++ )
  {
    if((*it).path == filePath)
    {
      return i;
    }
  }
  return -1;
}

KTextEditor::MarkInterface* EditorTabWidget::documentMarkIf(const QString& path)
{
  int index = documentIndex(path);
  if(index == -1) return NULL;

  return dynamic_cast<KTextEditor::MarkInterface*>(
           (*(m_docList.at(index))).view->document());
}


void EditorTabWidget::slotMarkChanged()
{
  //I get sick looking at this code...

  //terminating: prevent from crash
  if(m_terminating) return;

  QValueList<Document_t>::iterator it = m_docList.at(currentPageIndex());

  QPtrList<KTextEditor::Mark> currentMarks = documentMarkIf((*it).path)->marks();

  QValueList<KTextEditor::Mark> & oldMarks = (*it).marks;

  KTextEditor::Mark  *cur;
  QValueList<KTextEditor::Mark>::iterator  old;

  //closeGuard: prevent from emit signals to remove bp marks(we want them persistent)
  //m_markGuard: prevent from processing  marks twice
  if(m_closeGuard || m_markGuard) goto end;

  if(oldMarks.count() == currentMarks.count())
  {
    //--2+ marks on the same line | mark changed position
    KTextEditor::Mark mark;

    for(cur = currentMarks.first(), old = oldMarks.begin();
        cur;
        cur = currentMarks.next(), ++old)
    {

      if((cur->line == (*old).line) && (cur->type != (*old).type))
      {

        //--2+ marks on the same line

        mark.line = cur->line;
        if(cur->type > (*old).type)
        {
          mark.type = cur->type - (*old).type;
          //add
          dispatchMark(mark, true);
        }
        else
        {
          //remove
          mark.type = (*old).type - cur->type;
          dispatchMark(mark, false);
        }
        //break;

      }
      else if((*old).line != cur->line)
      {
        //mark changed position

        //remove old
        dispatchMark((*old), false);
        //add new
        dispatchMark(*cur, true);
      }
    }

  }
  else if(oldMarks.count() > currentMarks.count())
  {
    //a mark was removed
    if(currentMarks.count() == 0)
    {
      dispatchMark(*(oldMarks.begin()), false);
    }
    else
    {
      bool found;
      for(old = oldMarks.begin(); old != oldMarks.end(); ++old)
      {
        found = false;
        for(cur = currentMarks.first(); cur; cur = currentMarks.next())
        {
          if(((*old).line == cur->line) && ((*old).type |= cur->type))
          {
            found = true;
            break;
          }
        }
        if(found == false)
        {
          dispatchMark(*old, false);
          break;
        }
      }
    }
  }
  else
  {
    //a mark was added
    if(oldMarks.count() == 0)
    {
      dispatchMark(*(currentMarks.first()), true);
    }
    else
    {
      bool found;
      for(cur = currentMarks.first(); cur; cur = currentMarks.next())
      {
        found = false;
        for(old = oldMarks.begin(); old != oldMarks.end(); ++old)
        {
          if(((*old).line == cur->line) && ((*old).type |= cur->type))
          {
            found = true;
            break;
          }
        }
        if(found == false)
        {
          dispatchMark(*cur, true);
          break;
        }
      }
    }
  }

end:
  loadMarks((*it), (*it).view->document());
}


void EditorTabWidget::dispatchMark(KTextEditor::Mark& mark, bool adding)
{
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  if(!((mark.type & KTextEditor::MarkInterface::BreakpointDisabled) ||
       ((mark.type & KTextEditor::MarkInterface::BreakpointActive))))
  {
    return;
  }
#else
  if(!((mark.type & KTextEditor::MarkInterface::markType04) ||
       ((mark.type & KTextEditor::MarkInterface::markType02))))
  {
    return;
  }
#endif

  if(adding)
  {
    emit sigBreakpointMarked(currentDocumentPath(), mark.line);
  }
  else
  {
    emit sigBreakpointUnmarked(currentDocumentPath(), mark.line);
  }
}

void EditorTabWidget::loadMarks(Document_t& d, KTextEditor::Document* doc)
{
  d.marks.clear();

  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(doc);

  //this 'for' doesn't use first()/next() because it modifies the current item in the list
  //and it is already being used in the 'for' of slotMarkChanged()

  int size = imark->marks().count();
  for(int i = 0; i < size; i++)
  {
    d.marks.append((*imark->marks().at(i)));
  }
}

void EditorTabWidget::slotCurrentChanged(QWidget* w)
{
//   if(count())
//   {
//     KTextEditor::View* v = (*(m_docList.at(currentPageIndex()))).view;
//     m_partManager->setActivePart(v->document(), v);
//     m_partManager->setSelectedPart(v->document(), v);
//   }

  if(m_terminating) return;
  
  if(m_currentView)
  {
    m_window->guiFactory()->removeClient(m_currentView);
  }

  if(count())
  {
    m_currentView = (*(m_docList.at(currentPageIndex()))).view;
    m_window->guiFactory()->addClient(m_currentView);
  }
}

KTextEditor::View* EditorTabWidget::anyView()
{
  if(count() == 0) return 0;

  return m_docList.first().view;
}


#include "editortabwidget.moc"
