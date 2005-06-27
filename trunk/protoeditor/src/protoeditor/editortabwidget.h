/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
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

#include <ktabwidget.h>
#include <qvaluelist.h>
#include <ktexteditor/markinterface.h>

class MainWindow;
class KURL;
class Document;

namespace KTextEditor
{
  class View;
}

namespace KParts
{
  class Part;
}

class EditorTabWidget : public KTabWidget
{
  Q_OBJECT
public:
  EditorTabWidget(QWidget* parent, MainWindow *window, const char *name = 0);
  ~EditorTabWidget();

  //void setMainWindow(MainWindow*);

  bool openDocument(const KURL& url);
  bool closeCurrentDocument();
  bool closeAllDocuments();
  void setCurrentDocumentTab(const QString&, bool forceOpen = false);

  bool saveCurrentFile();
  bool saveCurrentFileAs(const KURL & url);

  void gotoLineAtFile(const QString& file, int line);

  const QString& documentPath(int index);

  const QString& currentDocumentPath();
  int            currentDocumentLine();

  void markActiveBreakpoint(const QString&, int);
  void unmarkActiveBreakpoint(const QString&, int);
  void markDisabledBreakpoint(const QString&, int);
  void unmarkDisabledBreakpoint(const QString&, int);
  void markExecutionPoint(const QString&, int);
  void unmarkExecutionPoint(const QString&);
  void markPreExecutionPoint(const QString&, int);
  void unmarkPreExecutionPoint(const QString&);

  bool hasBreakpointAt(const QString& , int);

  KTextEditor::View* currentView();

  void terminate();

signals:
  void sigBreakpointMarked(const QString&, int);
  void sigBreakpointUnmarked(const QString&, int);
  void sigNewDocument();
  void sigNoDocument();
  void sigAddWatch(const QString&);

public slots:
  void slotAddWatch();

protected slots:
  virtual void   closeRequest(int);
  virtual void   contextMenu(int, const QPoint &);
  
private slots:
  void slotCurrentChanged(QWidget*);
  void slotBreakpointMarked(Document* doc, int line);
  void slotBreakpointUnmarked(Document* doc, int line);

  void slotTextChanged();
  void slotStatusMsg(const QString&);

  void slotMenuAboutToShow();
private:
  bool                        closeDocument(int);
  Document*                   document(uint);
  Document*                   document(const QString&);
  Document*                   currentDocument();

  bool                        createDocument(const KURL& url);
  void                        setupMarks(KTextEditor::View* view);

  int                         documentIndex(const QString& filepath);
  //   void                        dispatchMark(KTextEditor::Mark& mark, bool adding);
  //   void                        loadMarks(Document_t&, KTextEditor::Document*);


  bool m_terminating;
  QValueList<Document*> m_docList;


  //bool m_markGuard;
  //we don't want sigBreakpointUnmarked() to be emited when the document close.
  //bool m_closeGuard;

  MainWindow* m_window;

  KTextEditor::View* m_currentView;
  //   KParts::PartManager* m_partManager;
};

#endif
