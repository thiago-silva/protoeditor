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

#ifndef MAINWINDOWBASE_H
#define MAINWINDOWBASE_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QTabWidget;
class QWidget;
class KTextEdit;
class QLabel;
class QComboBox;
class KListView;
class QListViewItem;
class QLineEdit;
class QPushButton;
class QStatusBar;

class EditorTabWidget;
class VariablesListView;
class WatchListView;
class LogListView;
class DebuggerComboStack;
class BreakpointListView;

class MainWindowBase : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowBase(QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel);
    virtual ~MainWindowBase();

  EditorTabWidget*    tabEditor();
  DebuggerComboStack* stackCombo();
  VariablesListView*  globalVarList();
  VariablesListView*  localVarList();

  QLineEdit* edAddWatch();
  QPushButton* btAddWatch();
  WatchListView*  watchList();
  BreakpointListView* breakpointListView();
  LogListView* logListView();
  KTextEdit* edOutput();


  QAction* fileOpenAction();
  QAction* fileSaveAction();
  QAction* fileCloseAction();
  QAction* fileExitAction();
  QAction* editUndoAction();
  QAction* editRedoAction();
  QAction* editCutAction();
  QAction* editCopyAction();
  QAction* editPasteAction();
  QAction* debuggerStartSessionAction();
  QAction* debuggerEndSessionAction();
  QAction* debuggerRunAction();
  QAction* debuggerStopAction();
  QAction* debuggerStepIntoAction();
  QAction* debuggerStepOverAction();
  QAction* debuggerStepOutAction();
  QAction* debuggerOptionsAction();
  QAction* configureEditorAction();
  QMenuBar* menuBar();

  QPopupMenu* menuFile();
  QPopupMenu* menuEdit();
  QPopupMenu* menuDebugger();
  QPopupMenu* menuEditor();

private:
    EditorTabWidget* m_tabEditor;
    DebuggerComboStack* m_stackCombo;
    VariablesListView* m_globaVarList;
    VariablesListView* m_localVarList;
    QLineEdit* m_edAddWatch;
    QPushButton* m_btAddWatch;
    WatchListView* m_watchList;
    BreakpointListView* m_breakpointList;
    LogListView* m_logListView;
    KTextEdit* m_edOutput;

    QAction* m_fileOpenAction;
    QAction* m_fileSaveAction;
    QAction* m_fileCloseAction;
    QAction* m_fileExitAction;
    QAction* m_editUndoAction;
    QAction* m_editRedoAction;
    QAction* m_editCutAction;
    QAction* m_editCopyAction;
    QAction* m_editPasteAction;
    QAction* m_debuggerStartSessionAction;
    QAction* m_debuggerEndSessionAction;
    QAction* m_debuggerRunAction;
    QAction* m_debuggerStopAction;
    QAction* m_debuggerStepIntoAction;
    QAction* m_debuggerStepOverAction;
    QAction* m_debuggerStepOutAction;
    QAction* m_debuggerOptionsAction;
    QAction* m_configureEditorAction;
    QMenuBar* m_menuBar;

    QPopupMenu* m_menuFile;
    QPopupMenu* m_menuEdit;
    QPopupMenu* m_menuDebugger;
    QPopupMenu* m_menuEditor;

    QStatusBar* m_statusBar;

};

#endif
