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

EditorTabWidget::EditorTabWidget(QWidget* parent, MainWindow *window, const char *name)
  : KTabWidget(parent, name), m_terminating(false),/* m_markGuard(false),m_closeGuard(false),*/
    m_window(window), m_currentView(0)
{
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


void EditorTabWidget::openDocument(const KURL& url)
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

  if(!m_terminating)
  {
    m_window->guiFactory()->removeClient(m_currentView);
    m_currentView = 0L;
  }

//   m_closeGuard = true;

  Document* doc = currentDocument();

  QWidget* w = page(currentPageIndex());
  removePage(w);

  m_docList.remove(doc);
  delete doc;

  if((count() == 0) && !m_terminating)
  {
    m_window->actionStateChanged("has_nofileopened");
  }

//   m_closeGuard = false;
}

void EditorTabWidget::closeAllDocuments()
{
  while(count())
  {
    closeCurrentDocument();
  }
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
  } else {
    return false;
  }
}

void EditorTabWidget::createDocument(const KURL& url)
{
  Document* doc = new Document(this);

  if(!doc->open(url))
  {
    delete doc;
    return;
  }

  connect(doc, SIGNAL(sigBreakpointMarked(Document*, int )), this,
          SLOT(slotBreakpointMarked(Document*, int)));
  
  connect(doc, SIGNAL(sigBreakpointUnmarked(Document*, int )), this,
          SLOT(slotBreakpointUnmarked(Document*, int)));
  
  insertTab(doc->tab(), url.fileName());
  KTextEditor::PopupMenuInterface* popupIf = dynamic_cast<KTextEditor::PopupMenuInterface*>(doc->view());
  if (popupIf)
    popupIf->installPopup((QPopupMenu *)m_window->factory()->container("ktexteditor_popup", m_window));

  m_docList.append(doc);

  setCurrentPage(count()-1);
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

void EditorTabWidget::slotCurrentChanged(QWidget*)
{
  if(m_terminating) return;

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


KTextEditor::View* EditorTabWidget::currentView()
{
  if(count() == 0) return 0;
  return m_currentView;
}

Document* EditorTabWidget::document(uint index)
{
  if(index > (m_docList.count()-1)) {
    return 0L;
  } else {
    return *(m_docList.at(index));
  }
}

Document* EditorTabWidget::document(const QString& filePath)
{
  QValueList<Document*>::iterator it;
  for(it = m_docList.begin(); it != m_docList.end(); ++it) {

    if((*it)->path() == filePath) {
      return (*it);
    }
  }

  return 0L;
}

Document* EditorTabWidget::currentDocument()
{
  return document(currentPageIndex());
}


#include "editortabwidget.moc"
