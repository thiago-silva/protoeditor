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
#include <qlayout.h>
#include "mainwindowbase.h"
#include "debuggerstack.h"
#include <ktexteditor/document.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/view.h>
#include <ktexteditor/markinterface.h>
#include <ktexteditor/markinterfaceextension.h>
#include <ktexteditor/configinterface.h>
#include <ktexteditor/viewcursorinterface.h>
#include <ktexteditor/clipboardinterface.h>
#include <ktexteditor/undointerface.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kglobal.h>
#include <qaction.h>
#include <klocale.h>

#include <qfile.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include "debuggerstack.h"

EditorTabWidget::EditorTabWidget(QWidget *parent, const char *name)
    : QTabWidget(parent, name), m_terminating(false), m_markGuard(false), m_window(0)
{
}

void EditorTabWidget::setMainWindow(MainWindowBase* window) {
  connect(window->editUndoAction(), SIGNAL(activated()),
          this, SLOT(slotUndo()));

  connect(window->editRedoAction(), SIGNAL(activated()),
          this, SLOT(slotRedo()));

  connect(window->editCutAction(), SIGNAL(activated()),
          this, SLOT(slotCut()));

  connect(window->editCopyAction(), SIGNAL(activated()),
          this, SLOT(slotCopy()));

  connect(window->editPasteAction(), SIGNAL(activated()),
          this, SLOT(slotPaste()));

  connect(window->configureEditorAction(), SIGNAL(activated()),
          this, SLOT(slotConfigEditor()));
  m_window = window;
  disableEditMenu();
  disableConfigEditor();
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


void EditorTabWidget::addDocument(QUrl url)
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
  }

  enableEditMenu();
  enableConfigEditor();
}

void EditorTabWidget::closeCurrentDocument()
{
  if(count() == 0) return;

  int index = currentPageIndex();

  QValueList<Document_t>::iterator it = m_docList.at(index);

  KTextEditor::Document* doc = (*it).view->document();
  doc->closeURL();
  KTextEditor::View* view = (*it).view;
  delete view;
  delete doc;
  QWidget* w = page(index);
  removePage(w);

  m_docList.remove(it);

  if(count() == 0)
  {
    disableEditMenu();
    disableConfigEditor();
  }
}

void EditorTabWidget::closeAllDocuments()
{
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
  disableEditMenu();
  disableConfigEditor();
}

void EditorTabWidget::setCurrentDocument(QString filePath, bool forceOpen)
{
  int index = documentIndex(filePath);
  if(index != -1)
  {
    setCurrentPage(index);
  }
  else if(forceOpen)
  {
    createDocument(QUrl(filePath));
  }
}

bool EditorTabWidget::saveCurrentFile()
{
  if(count() == 0) return false;

  KTextEditor::Document *doc = (*(m_docList.at(currentPageIndex()))).view->document();
  return doc->save();
}

void EditorTabWidget::gotoLineAtFile(QString file, int line)
{
  int index;
  KTextEditor::View* view;
  KTextEditor::ViewCursorInterface *vci;

  Document_t d;
  if((index = documentIndex(file)) != -1)
  {
    d = *(m_docList.at(index));
    view = d.view;

    vci = dynamic_cast<KTextEditor::ViewCursorInterface*>(view);
    vci->setCursorPosition(line, 0);
  }
}


QString EditorTabWidget::documentPath(int index)
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

void EditorTabWidget::markActiveBreakpoint(QString filepath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filepath);
  if(!imark) return;
  m_markGuard = true;
  imark->addMark(line-1, ActiveBreakpoint);
  m_markGuard = false;
}

void EditorTabWidget::unmarkActiveBreakpoint(QString filepath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filepath);
  m_markGuard = true;
  if(!imark) return;
  imark->removeMark(line-1, ActiveBreakpoint);
  m_markGuard = false;
}

void EditorTabWidget::markInactiveBreakpoint(QString filepath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filepath);
  m_markGuard = true;
  if(!imark) return;
  imark->addMark(line-1, InactiveBreakpoint);
  m_markGuard = false;
}

void EditorTabWidget::unmarkInactiveBreakpoint(QString filepath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filepath);
  m_markGuard = true;
  if(!imark) return;
  imark->removeMark(line-1, InactiveBreakpoint);
  m_markGuard = false;
}

void EditorTabWidget::markExecutionLine(QString filepath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filepath);
  if(!imark) return;
  imark->addMark(line-1, CurrentExecutionLine);
}

void EditorTabWidget::unmarkExecutionLine(QString filepath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filepath);
  if(!imark) return;
  imark->removeMark(line-1, CurrentExecutionLine);
}

void EditorTabWidget::markPreExecutionLine(QString filepath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filepath);
  if(!imark) return;
  imark->addMark(line-1, PreExecutionLine);
}

void EditorTabWidget::unmarkPreExecutionLine(QString filepath, int line)
{
  KTextEditor::MarkInterface* imark = documentMarkIf(filepath);
  if(!imark) return;
  imark->removeMark(line-1, PreExecutionLine);
}

void EditorTabWidget::createDocument(QUrl url/*, QString text*/)
{

  KTextEditor::View *view = openKDocument(url);

  KTextEditor::MarkInterfaceExtension* imarkex =
    dynamic_cast<KTextEditor::MarkInterfaceExtension*>(view->document());

  if(imarkex)
  {
    KIconLoader *loader = KGlobal::iconLoader();

    imarkex->setPixmap(KTextEditor::MarkInterface::markType05, loader->loadIcon(
                         "debugger-currentline", KIcon::User));

    imarkex->setPixmap(KTextEditor::MarkInterface::markType06, loader->loadIcon(
                         "debugger-stackcontext", KIcon::User));

    imarkex->setPixmap(KTextEditor::MarkInterface::markType03, loader->loadIcon(
                         "debugger-activebreakpoint",KIcon::User));

    imarkex->setPixmap(KTextEditor::MarkInterface::markType04, loader->loadIcon(
                         "debugger-inactivebreakpoint",KIcon::User));

    imarkex->setDescription(KTextEditor::MarkInterface::markType03, "Breakpoint");

    imarkex->setMarksUserChangable(Bookmark + ActiveBreakpoint);
  }

  connect(view->document(), SIGNAL(marksChanged()), this, SLOT(slotMarkChanged()));


  Document_t d;
  d.path = url.path();
  d.view = view;
  //d.marks = imark->marks();
  m_docList.append(d);
}

int EditorTabWidget::documentIndex(QString filepath)
{
  int i = 0;

  QValueList<Document_t>::iterator it;
  for (it = m_docList.begin(); it != m_docList.end(); ++it, i++ )
  {
    if((*it).path == filepath)
    {
      return i;
    }
  }
  return -1;
}

KTextEditor::View* EditorTabWidget::openKDocument(QUrl url)
{
  QWidget* tab = new QWidget(this);
  QVBoxLayout *lay = new QVBoxLayout(tab, 11, 6);

  KTextEditor::Document *doc =
    KTextEditor::EditorChooser::createDocument(
      0L, "KTextEditor::Document");

  doc->openURL(url);

  KTextEditor::View * view = doc->createView(tab);

  lay->addWidget(view);
  insertTab(tab, url.fileName());
  setCurrentPage(count()-1);
  return view;
}

KTextEditor::MarkInterface* EditorTabWidget::documentMarkIf(QString path)
{
  int index = documentIndex(path);
  if(index == -1) return NULL;

  return dynamic_cast<KTextEditor::MarkInterface*>(
           (*(m_docList.at(index))).view->document());
}

/*
 * Slots
 */

void EditorTabWidget::slotUndo()
{
  KTextEditor::Document *doc = (*(m_docList.at(currentPageIndex()))).view->document();

  KTextEditor::UndoInterface* undoif =
    dynamic_cast<KTextEditor::UndoInterface*>(doc);
  if(undoif) undoif->undo();
}

void EditorTabWidget::slotRedo()
{
  KTextEditor::Document *doc = (*(m_docList.at(currentPageIndex()))).view->document();

  KTextEditor::UndoInterface* undoif =
    dynamic_cast<KTextEditor::UndoInterface*>(doc);
  if(undoif) undoif->redo();
}

void EditorTabWidget::slotCut()
{
  KTextEditor::View *v = (*(m_docList.at(currentPageIndex()))).view;

  KTextEditor::ClipboardInterface* clip =
    dynamic_cast<KTextEditor::ClipboardInterface*>(v);
  if(clip)
  {
    clip->cut();
  }
}
void EditorTabWidget::slotCopy()
{
  KTextEditor::View *v = (*(m_docList.at(currentPageIndex()))).view;

  KTextEditor::ClipboardInterface* clip =
    dynamic_cast<KTextEditor::ClipboardInterface*>(v);
  if(clip)
  {
    clip->copy();
  }
}

void EditorTabWidget::slotPaste()
{
  KTextEditor::View *v = (*(m_docList.at(currentPageIndex()))).view;

  KTextEditor::ClipboardInterface* clip =
    dynamic_cast<KTextEditor::ClipboardInterface*>(v);
  if(clip)
  {
    clip->paste();
  }
}

void EditorTabWidget::slotConfigEditor()
{
  if(count() == 0) return;

  KTextEditor::Document *doc = (*m_docList.at(currentPageIndex())).view->document();
  if(!doc) return;

  KTextEditor::ConfigInterface* configIf = dynamic_cast<KTextEditor::ConfigInterface*>(doc);
  configIf->configDialog();
}

void EditorTabWidget::slotMarkChanged()
{
  if(m_terminating) return;

  //1-find out wich document is active
  //2- analyse its previous marks with its current marks
  QValueList<Document_t>::iterator it = m_docList.at(currentPageIndex());

  QPtrList<KTextEditor::Mark> currentMarks = documentMarkIf((*it).path)->marks();

  QValueList<KTextEditor::Mark> & oldMarks = (*it).marks;

  KTextEditor::Mark  *cur;
  QValueList<KTextEditor::Mark>::iterator  old;


  if(oldMarks.count() == currentMarks.count()) return;

  if(oldMarks.count() > currentMarks.count())
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

  loadMarks((*it), (*it).view->document());
}


void EditorTabWidget::dispatchMark(KTextEditor::Mark& mark, bool adding)
{
  if(m_markGuard == true) return;

  if(!((mark.type & InactiveBreakpoint) ||
       ((mark.type & ActiveBreakpoint))))
  {
    return;
  }

  if(adding)
  {
    emit sigBreakpointMarked(
      documentPath(currentPageIndex()), mark.line);
  }
  else
  {
    emit sigBreakpointUnmarked(
      documentPath(currentPageIndex()), mark.line);
  }
  /*
   Document_t d = *(m_docList.at(currentPageIndex()));
   for(int i = 0; i < d.marks.count(); i++) {
     if((*(d.marks.at(i))).line == mark.line) {
       mark.line ^= (*(d.marks.at(i))).type;
     }
   }
   */


  /*
    switch(mark.type) {
      case Bookmark:
        kdDebug() << t << "bookmark: " << mark.line << "\n";
        break;
      case CurrentExecutionLine:
        kdDebug() << t << "CurrentExecutionLine: "  << mark.line << "\n";
          break;
      case PreExecutionLine:
        kdDebug() << t << "PreExecutionLine: "  << mark.line << "\n";;
          break;
      case ActiveBreakpoint:
        kdDebug() << t << "ActiveBreakpoint: "  << mark.line << "\n";;
          break;
      case InactiveBreakpoint:
        kdDebug() << t << "InactiveBreakpoint";
          break;
    }
  */
}

void EditorTabWidget::loadMarks(Document_t& d, KTextEditor::Document* doc)
{
  /*
    QString s = "result: ";
    for(int i = 0; i < d.marks.count(); i++) {
      s += "(" + QString::number((*(d.marks.at(i))).line+1) + ")*" + QString::number((*(d.marks.at(i))).type) + "*; ";
    }
    kdDebug() << s;
  */

  d.marks.clear();

  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(doc);

  //this 'for' doesn't use first()/next() because it modifies the current item in the list
  //and it is already being used in the 'for' of slotMarkChanged()

  //QString s = "result: ";
  int size = imark->marks().count();
  for(int i = 0; i < size; i++)
  {
    //s += "(" + QString::number(m->line+1) + ")*" + QString::number(m->type) + "*; ";
    d.marks.append((*imark->marks().at(i)));
  }
  //kdDebug() << s;
}


void EditorTabWidget::disableEditMenu()
{
  m_window->editCopyAction()->setEnabled(false);
  m_window->editCutAction()->setEnabled(false);
  m_window->editPasteAction()->setEnabled(false);
  m_window->editRedoAction()->setEnabled(false);
  m_window->editUndoAction()->setEnabled(false);
}

void EditorTabWidget::enableEditMenu()
{
  m_window->editCopyAction()->setEnabled(true);
  m_window->editCutAction()->setEnabled(true);
  m_window->editPasteAction()->setEnabled(true);
  m_window->editRedoAction()->setEnabled(true);
  m_window->editUndoAction()->setEnabled(true);
}

void EditorTabWidget::disableConfigEditor()
{
  m_window->configureEditorAction()->setEnabled(false);
}

void EditorTabWidget::enableConfigEditor()
{
  m_window->configureEditorAction()->setEnabled(true);
}


#include "editortabwidget.moc"
