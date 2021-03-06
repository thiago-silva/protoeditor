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

#ifndef EDITORTABWIDGET_H
#define EDITORTABWIDGET_H

#include "editorinterface.h"

#include <ktabwidget.h>
#include <qvaluelist.h>
#include <ktexteditor/markinterface.h>

class KURL;
class Document;
class KAction;
class MainWindow;

namespace KTextEditor
{
  class View;
}

namespace KParts
{
  class Part;
}

class EditorUI : public KTabWidget,
                 public EditorInterface
{
  Q_OBJECT
public:
  EditorUI(QWidget* parent, MainWindow*, const char *name = 0);
  ~EditorUI();

  void createNew();
  bool openDocument(const KURL& url);
  bool closeCurrentDocument();
  bool closeAllDocuments();
  void activateDocument(const KURL&, bool forceOpen = false);

  bool saveCurrentFile();
  bool saveCurrentFileAs(const KURL & url, const QString& encoding = QString::null);

  void gotoLineAtFile(const KURL&, int);

  KURL documentURL(int index);

  KURL currentDocumentURL();
  int  currentDocumentLine();
  bool currentDocumentExistsOnDisk();

  void markActiveBreakpoint(const KURL&, int);
  void unmarkActiveBreakpoint(const KURL&, int);
  void markDisabledBreakpoint(const KURL&, int);
  void unmarkDisabledBreakpoint(const KURL&, int);
  void markExecutionPoint(const KURL&, int);
  void unmarkExecutionPoint(const KURL&);
  void markPreExecutionPoint(const KURL&, int);
  void unmarkPreExecutionPoint(const KURL&);

  bool hasBreakpointAt(const KURL& , int);

  KTextEditor::View* currentView();

  void terminate();

  void saveExistingFiles();

  int totalDocuments();
signals:
  void sigBreakpointMarked(const KURL&, int, bool);
  void sigBreakpointUnmarked(const KURL&, int);
  void sigNewPage();
  void sigEmpty();
  void sigFirstPage();
  void sigAddWatch(const QString&);

protected slots:
  virtual void   closeRequest(int);
  virtual void   contextMenu(int, const QPoint &);
  

private slots:  
  void slotCurrentChanged(QWidget*);
  void slotBreakpointMarked(Document* doc, int line, bool enabled);
  void slotBreakpointUnmarked(Document* doc, int line);

  void slotTextChanged();
  void slotStatusMsg(const QString&);

  void slotMenuAboutToShow();
  void slotAddWatch();

  void slotDropEvent(QDropEvent*);

  void slotDocumentSaved(Document*);

  void slotActivateNextTab();
  void slotActivatePrevTab();
  void slotFocusCurrentDocument();

  void slotConfigureEditor();
protected:
  void dragEnterEvent(QDragEnterEvent*);
  void dragMoveEvent( QDragMoveEvent *);
  void dropEvent(QDropEvent*);

private:
  void                        initDoc(Document*);
  bool                        closeDocument(int);
  Document*                   document(unsigned int );
  Document*                   document(const KURL&);
  Document*                   currentDocument();
  void                        createDocument();
  bool                        createDocument(const KURL& url);
  void                        setupMarks(KTextEditor::View* view);

  int                         documentIndex(const KURL&);

  MainWindow           *m_window;
  KTextEditor::View    *m_currentView;  
  bool                  m_terminating;
  QValueList<Document*> m_docList;
  
};

#endif
