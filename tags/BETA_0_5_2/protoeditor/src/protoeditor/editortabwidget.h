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
  void setCurrentDocument(QString, bool forceOpen = false);

  bool saveCurrentFile();

  void gotoLineAtFile(QString file, int line);

  QString documentPath(int index);

  void markActiveBreakpoint(QString, int);
  void unmarkActiveBreakpoint(QString, int);
  void markDisabledBreakpoint(QString, int);
  void unmarkDisabledBreakpoint(QString, int);
  void markExecutionLine(QString, int);
  void unmarkExecutionLine(QString, int);
  void markPreExecutionLine(QString, int);
  void unmarkPreExecutionLine(QString, int);


  KTextEditor::View* anyView();

  void init();

  void terminate();

signals:
  void sigBreakpointMarked(QString, int);
  void sigBreakpointUnmarked(QString, int);

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
  void enableEditorActions();
  void disableEditorActions();
  void enableUndoAction();
  void disableUndoAction();
  void enableRedoAction();
  void disableRedoAction();


//  void disableEditMenu();
//  void enableEditMenu();

//  void disableConfigEditor();
//  void enableConfigEditor();

  typedef  struct {
      QString path;
      KTextEditor::View* view;
      QValueList<KTextEditor::Mark> marks;
      bool hasUndo;
      bool hasRedo;
  } Document_t;

  void createDocument(KURL url);

  int documentIndex(QString filepath);

  KTextEditor::View*          openKDocument(KURL);

  KTextEditor::MarkInterface* documentMarkIf(QString);

  void dispatchMark(KTextEditor::Mark& mark, bool adding);

  void loadMarks(Document_t&, KTextEditor::Document*);

  bool m_terminating;
  QValueList<Document_t> m_docList;

  bool m_markGuard;
  MainWindow* m_window;
};

#endif
