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
#include <qurl.h>
#include <qvaluelist.h>
#include <ktexteditor/markinterface.h>
#include <qpair.h>

class KTextEdit;
class MainWindowBase;

namespace KTextEditor {
  class View;
}

class DebuggerStack;

class EditorTabWidget : public KTabWidget
{
Q_OBJECT
public:
  EditorTabWidget(QWidget *parent = 0, const char *name = 0);
  ~EditorTabWidget();

  //void setMainWindow(MainWindow*);

  void addDocument(QUrl url);
  void closeCurrentDocument();
  void closeAllDocuments();
  void setCurrentDocument(QString, bool forceOpen = false);

  bool saveCurrentFile();

  void gotoLineAtFile(QString file, int line);

  QString documentPath(int index);

  void markActiveBreakpoint(QString, int line);
  void unmarkActiveBreakpoint(QString, int line);
  void markInactiveBreakpoint(QString, int line);
  void unmarkInactiveBreakpoint(QString, int line);
  void markExecutionLine(QString, int line);
  void unmarkExecutionLine(QString, int line);
  void markPreExecutionLine(QString, int line);
  void unmarkPreExecutionLine(QString, int line);


  KTextEditor::View* anyView();

  void terminate();

signals:
  void sigBreakpointMarked(QString, int);
  void sigBreakpointUnmarked(QString, int);
  void sigHasNoFiles();
  void sigHasFiles();

  void sigHasNoUndo();
  void sigHasUndo();

  void sigHasNoRedo();
  void sigHasRedo();

  //void sigTabChanged(int);

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

  enum {
    Bookmark             = KTextEditor::MarkInterface::markType01,
    CurrentExecutionLine = KTextEditor::MarkInterface::markType05,
    PreExecutionLine     = KTextEditor::MarkInterface::markType06,
    ActiveBreakpoint     = KTextEditor::MarkInterface::markType03,
    InactiveBreakpoint   = KTextEditor::MarkInterface::markType04
  };

  void createDocument(QUrl url);

  int documentIndex(QString filepath);

  KTextEditor::View*          openKDocument(QUrl);

  KTextEditor::MarkInterface* documentMarkIf(QString);

  void dispatchMark(KTextEditor::Mark& mark, bool adding);

  void loadMarks(Document_t&, KTextEditor::Document*);

  bool m_terminating;
  QValueList<Document_t> m_docList;

  bool m_markGuard;
  MainWindowBase* m_window;
};

#endif
