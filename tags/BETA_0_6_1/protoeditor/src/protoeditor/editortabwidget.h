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

#ifndef EDITORTABWIDGET_H
#define EDITORTABWIDGET_H

#include <ktabwidget.h>
#include <kurl.h>
#include <qvaluelist.h>
#include <ktexteditor/markinterface.h>
#include <qpair.h>

class KTextEdit;
class MainWindow;

namespace KTextEditor {
  class View;
}

class EditorTabWidget : public KTabWidget
{
Q_OBJECT
public:
  EditorTabWidget(QWidget* parent, MainWindow *window, const char *name = 0);
  ~EditorTabWidget();

  //void setMainWindow(MainWindow*);

  void addDocument(KURL url);
  void closeCurrentDocument();
  void closeAllDocuments();
  void setCurrentDocument(const QString&, bool forceOpen = false);

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
  void unmarkExecutionPoint(const QString&, int);
  void markPreExecutionPoint(const QString&, int);
  void unmarkPreExecutionPoint(const QString&, int);

  bool hasBreakpointAt(const QString& , int);

  KTextEditor::View* anyView();

  void terminate();

signals:
  void sigBreakpointMarked(const QString&, int);
  void sigBreakpointUnmarked(const QString&, int);
  void sigNewDocument();

/*
  void sigHasNoFiles();
  void sigHasFiles();

  void sigHasNoUndo();
  void sigHasUndo();

  void sigHasNoRedo();
  void sigHasRedo();
*/

public slots:
    void slotUndo();
    void slotRedo();
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotSelectAll();
    /*
    void slotSearch();
    void slotSearchAgain();
    void slotReplace();
    void slotGotoLine();
    */
    void slotConfigEditor();

private slots:
    void slotMarkChanged();
    void slotUndoChanged();

    void slotCurrentChanged(QWidget*);

private:

  typedef  struct {
      QString path;
      KTextEditor::View* view;
      QValueList<KTextEditor::Mark> marks;
      bool hasUndo;
      bool hasRedo;
  } Document_t;


  void                        createDocument(KURL url);
  int                         documentIndex(const QString& filepath);
  KTextEditor::View*          openKDocument(KURL);
  KTextEditor::MarkInterface* documentMarkIf(const QString&);
  void                        dispatchMark(KTextEditor::Mark& mark, bool adding);
  void                        loadMarks(Document_t&, KTextEditor::Document*);

  void enableEditorActions();
  void disableEditorActions();
  void enableUndoAction();
  void disableUndoAction();
  void enableRedoAction();
  void disableRedoAction();


  bool m_terminating;
  QValueList<Document_t> m_docList;

  //
  bool m_markGuard;
  //we don't want sigBreakpointUnmarked() to be emited when the document close.
  bool m_closeGuard;

  MainWindow* m_window;
};

#endif
