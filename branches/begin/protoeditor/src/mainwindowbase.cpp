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

#include "mainwindowbase.h"
#include "editortabwidget.h"
#include "variableslistview.h"
#include "watchlistview.h"
#include "loglistview.h"
#include "debuggercombostack.h"
#include "breakpointlistview.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <ktextedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qheader.h>
#include <klistview.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <klocale.h>


MainWindowBase::MainWindowBase(QWidget* parent, const char* name, WFlags fl)
    : QMainWindow(parent, name, fl)
{
  //(void)statusBar();

  if(!name) {
    setName("MainFormBase");
  }

    setCentralWidget(new QWidget(this, "qt_central_widget"));
    QVBoxLayout* mainFormBaseLayout = new QVBoxLayout(centralWidget(), 11, 6);

    m_tabEditor = new EditorTabWidget(centralWidget());
    m_tabEditor->setMinimumSize(QSize(795, 504));
    mainFormBaseLayout->addWidget(m_tabEditor);

    QTabWidget* tabDebug = new QTabWidget(centralWidget());
    tabDebug->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, tabDebug->sizePolicy().hasHeightForWidth()));
    tabDebug->setMinimumSize(QSize(745, 300));

    QWidget* globalVarTab = new QWidget(tabDebug);
    QVBoxLayout* globalVarTabLayout = new QVBoxLayout(globalVarTab, 11, 6);

    m_globaVarList = new VariablesListView(globalVarTab);
    globalVarTabLayout->addWidget(m_globaVarList);
    tabDebug->insertTab(globalVarTab, QString("Global"));

    QWidget* tabStack = new QWidget(tabDebug);
    QVBoxLayout* varTabLayout = new QVBoxLayout(tabStack, 11, 6);
    QHBoxLayout* stackComboLayout = new QHBoxLayout(0, 0, 6);

    QLabel* stackLabel = new QLabel(tabStack);
    stackLabel->setText(tr2i18n("Stack:"));
    stackLabel->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, stackLabel->sizePolicy().hasHeightForWidth()));
    stackComboLayout->addWidget(stackLabel);

    m_stackCombo = new DebuggerComboStack(tabStack);
    stackComboLayout->addWidget(m_stackCombo);
    varTabLayout->addLayout(stackComboLayout);

    m_localVarList= new VariablesListView(tabStack);
    varTabLayout->addWidget(m_localVarList);
    tabDebug->insertTab(tabStack, QString("Local"));

    QWidget* tabWatch = new QWidget(tabDebug);
    QVBoxLayout* watchTabLayout = new QVBoxLayout(tabWatch, 11, 6);

    QHBoxLayout* addWatchLayout = new QHBoxLayout(0, 0, 6);

    QLabel* addLabel = new QLabel(tabWatch);
    addLabel->setText(tr2i18n("Watch:"));
    addWatchLayout->addWidget(addLabel);

    m_edAddWatch = new QLineEdit(tabWatch);
    m_edAddWatch->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_edAddWatch->sizePolicy().hasHeightForWidth()));
    addWatchLayout->addWidget(m_edAddWatch);

    m_btAddWatch = new QPushButton(tabWatch);
    m_btAddWatch->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_btAddWatch->sizePolicy().hasHeightForWidth()));
    m_btAddWatch->setText(tr2i18n("Add"));
    addWatchLayout->addWidget(m_btAddWatch);

    QSpacerItem* spacer = new QSpacerItem(430, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    addWatchLayout->addItem(spacer);
    watchTabLayout->addLayout(addWatchLayout);

    m_watchList = new WatchListView(tabWatch);
    watchTabLayout->addWidget(m_watchList);
    tabDebug->insertTab(tabWatch, QString("Watch"));

    QWidget* breakpointTab = new QWidget(tabDebug);
    QVBoxLayout* breakpointTabLayout = new QVBoxLayout(breakpointTab, 11, 6);

    m_breakpointList = new BreakpointListView(breakpointTab);
    breakpointTabLayout->addWidget(m_breakpointList);
    tabDebug->insertTab(breakpointTab, QString("Breakpoints"));

    QWidget* logTab = new QWidget(tabDebug);
    QVBoxLayout* logTabLayout = new QVBoxLayout(logTab, 11, 6);

    m_logListView = new LogListView(logTab);
    logTabLayout->addWidget(m_logListView);
    tabDebug->insertTab(logTab, QString("log"));

    QWidget* outputTab = new QWidget(tabDebug);
    QVBoxLayout* outputTabLayout = new QVBoxLayout(outputTab, 11, 6);

    m_edOutput = new KTextEdit(outputTab);
    outputTabLayout->addWidget(m_edOutput);
    m_edOutput->setReadOnly(true);
    tabDebug->insertTab(outputTab, QString("Output"));
    mainFormBaseLayout->addWidget(tabDebug);

    // actions
    m_fileOpenAction = new QAction(this);
    m_fileOpenAction->setText(tr2i18n("Open"));
    m_fileOpenAction->setMenuText(tr2i18n("Open"));
    m_fileOpenAction->setAccel(QKeySequence(tr("Ctrl+O", "")));

    m_fileSaveAction = new QAction(this);
    m_fileSaveAction->setText(tr2i18n("Save"));
    m_fileSaveAction->setMenuText(tr2i18n("Save"));
    m_fileSaveAction->setAccel(QKeySequence(tr("Ctrl+S", "")));

    m_fileCloseAction = new QAction(this);
    m_fileCloseAction->setText(tr2i18n("Close"));
    m_fileCloseAction->setMenuText(tr2i18n("Close"));
    m_fileCloseAction->setAccel(QKeySequence(tr("Ctrl+W", "File|Close")));

    m_fileExitAction = new QAction(this);
    m_fileExitAction->setText(tr2i18n("Exit"));
    m_fileExitAction->setMenuText(tr2i18n("Exit"));


    m_editUndoAction = new QAction(this);
    m_editUndoAction->setText(tr2i18n("Undo"));
    m_editUndoAction->setMenuText(tr2i18n("undo"));
    m_editUndoAction->setAccel(QKeySequence(tr("Ctrl+Z", "")));

    m_editRedoAction = new QAction(this);
    m_editRedoAction->setText(tr2i18n("Redo"));
    m_editRedoAction->setMenuText(tr2i18n("Redo"));
    m_editRedoAction->setAccel(QKeySequence(tr("Ctrl+Shift+Z", "")));

    m_editCutAction = new QAction(this);
    m_editCutAction->setText(tr2i18n("Cut"));
    m_editCutAction->setMenuText(tr2i18n("Cut"));
    m_editCutAction->setAccel(QKeySequence(tr("Ctrl+X", "")));

    m_editCopyAction = new QAction(this);
    m_editCopyAction->setText(tr2i18n("Copy"));
    m_editCopyAction->setMenuText(tr2i18n("Copy"));
    m_editCopyAction->setAccel(QKeySequence(tr("Ctrl+C", "")));

    m_editPasteAction = new QAction(this);
    m_editPasteAction->setText(tr2i18n("Paste"));
    m_editPasteAction->setMenuText(tr2i18n("Paste"));
    m_editPasteAction->setAccel(QKeySequence(tr("Ctrl+V", "")));

    m_debuggerOptionsAction = new QAction(this);
    m_debuggerOptionsAction->setText(tr2i18n("Options"));
    m_debuggerOptionsAction->setMenuText(tr2i18n("Options"));

    m_debuggerStartSessionAction = new QAction(this);
    m_debuggerStartSessionAction->setText(tr2i18n("Start Session"));
    m_debuggerStartSessionAction->setMenuText(tr2i18n("Start Session"));
    m_debuggerStartSessionAction->setAccel(QKeySequence(tr("F1", "")));

    m_debuggerEndSessionAction = new QAction(this);;
    m_debuggerEndSessionAction->setText(tr2i18n("End Session"));
    m_debuggerEndSessionAction->setMenuText(tr2i18n("End Session"));
    m_debuggerEndSessionAction->setAccel(QKeySequence(tr("F2", "")));

    m_debuggerRunAction = new QAction(this);
    m_debuggerRunAction->setText(tr2i18n("Run"));
    m_debuggerRunAction->setMenuText(tr2i18n("Run"));
    m_debuggerRunAction->setAccel(QKeySequence(tr("F3", "")));

    m_debuggerStopAction = new QAction(this);
    m_debuggerStopAction->setText(tr2i18n("Stop"));
    m_debuggerStopAction->setMenuText(tr2i18n("Stop"));
    m_debuggerStopAction->setAccel(QKeySequence(tr("F4", "")));

    m_debuggerStepIntoAction = new QAction(this);
    m_debuggerStepIntoAction->setText(tr2i18n("Step into"));
    m_debuggerStepIntoAction->setMenuText(tr2i18n("Step into"));
    m_debuggerStepIntoAction->setAccel(QKeySequence(tr("F5", "")));

    m_debuggerStepOverAction = new QAction(this);
    m_debuggerStepOverAction->setText(tr2i18n("Step over"));
    m_debuggerStepOverAction->setMenuText(tr2i18n("Step over"));
    m_debuggerStepOverAction->setAccel(QKeySequence(tr("F6", "")));

    m_debuggerStepOutAction = new QAction(this);
    m_debuggerStepOutAction->setText(tr2i18n("Step out"));
    m_debuggerStepOutAction->setMenuText(tr2i18n("Step out"));
    m_debuggerStepOutAction->setAccel(QKeySequence(tr("F7", "")));

    // menubar
    m_menuBar = new QMenuBar(this);

    m_menuFile = new QPopupMenu(this);
    m_fileOpenAction->addTo(m_menuFile);
    m_fileSaveAction->addTo(m_menuFile);
    m_fileCloseAction->addTo(m_menuFile);
    m_fileExitAction->addTo(m_menuFile);
    m_menuBar->insertItem(QString("File"), m_menuFile, 1);


    m_menuEdit = new QPopupMenu(this);
    m_editUndoAction->addTo(m_menuEdit);
    m_editRedoAction->addTo(m_menuEdit);
    m_editCutAction->addTo(m_menuEdit);
    m_editCopyAction->addTo(m_menuEdit);
    m_editPasteAction->addTo(m_menuEdit);
    m_menuEdit->insertSeparator(2);
    m_menuBar->insertItem(QString("Edit"), m_menuEdit, 2);


    m_menuDebugger = new QPopupMenu(this);
    m_debuggerOptionsAction->addTo(m_menuDebugger);
    m_debuggerStartSessionAction->addTo(m_menuDebugger);
    m_debuggerEndSessionAction->addTo(m_menuDebugger);
    m_debuggerRunAction->addTo(m_menuDebugger);
    m_debuggerStopAction->addTo(m_menuDebugger);
    m_debuggerStepIntoAction->addTo(m_menuDebugger);
    m_debuggerStepOverAction->addTo(m_menuDebugger);
    m_debuggerStepOutAction->addTo(m_menuDebugger);
    m_menuDebugger->insertSeparator(1);
    m_menuDebugger->insertSeparator(4);
    m_menuBar->insertItem(QString("Debugger"), m_menuDebugger, 3);

    m_configureEditorAction = new QAction(this);
    m_configureEditorAction->setText(tr2i18n("Configure Editor"));
    m_configureEditorAction->setMenuText(tr2i18n("Configure Editor"));

    m_menuEditor = new QPopupMenu(this);
    m_configureEditorAction->addTo(m_menuEditor);
    m_menuBar->insertItem(QString("Editor"), m_menuEditor, 4);

    m_statusBar = new QStatusBar(this);

    resize(QSize(817, 776).expandedTo(minimumSizeHint()));
    clearWState(WState_Polished);

    m_tabEditor->setMainWindow(this);
}

/*
*  Destroys the object and frees any allocated resources
*/
MainWindowBase::~MainWindowBase()
{
    // no need to delete child widgets, Qt does it all for us
}


EditorTabWidget* MainWindowBase::tabEditor() {
  return m_tabEditor;
}

DebuggerComboStack* MainWindowBase::stackCombo() {
  return m_stackCombo;
}

VariablesListView*  MainWindowBase::globalVarList() {
  return m_globaVarList;
}

VariablesListView*  MainWindowBase::localVarList() {
  return m_localVarList;
}

QLineEdit* MainWindowBase::edAddWatch() {
  return m_edAddWatch;
}

QPushButton* MainWindowBase::btAddWatch() {
  return m_btAddWatch;
}

WatchListView*  MainWindowBase::watchList() {
  return m_watchList;
}

BreakpointListView* MainWindowBase::breakpointListView() {
  return m_breakpointList;
}

LogListView* MainWindowBase::logListView() {
  return m_logListView;
}

KTextEdit* MainWindowBase::edOutput() {
  return m_edOutput;
}

QAction* MainWindowBase::fileOpenAction() {
  return m_fileOpenAction;
}

QAction* MainWindowBase::fileSaveAction() {
  return m_fileSaveAction;
}

QAction* MainWindowBase::fileCloseAction() {
  return m_fileCloseAction;
}

QAction* MainWindowBase::fileExitAction() {
  return m_fileExitAction;
}

QAction* MainWindowBase::debuggerStartSessionAction() {
  return m_debuggerStartSessionAction;
}

QAction* MainWindowBase::debuggerEndSessionAction() {
  return m_debuggerEndSessionAction;
}

QAction* MainWindowBase::debuggerRunAction() {
  return m_debuggerRunAction;
}

QAction* MainWindowBase::debuggerStopAction() {
  return m_debuggerStopAction;
}

QAction* MainWindowBase::debuggerStepIntoAction() {
  return m_debuggerStepIntoAction;
}

QAction* MainWindowBase::debuggerStepOverAction() {
  return m_debuggerStepOverAction;
}

QAction* MainWindowBase::debuggerStepOutAction() {
  return m_debuggerStepOutAction;
}

QAction* MainWindowBase::debuggerOptionsAction() {
  return m_debuggerOptionsAction;
}

QAction* MainWindowBase::configureEditorAction() {
  return m_configureEditorAction;
}

QMenuBar* MainWindowBase::menuBar() {
  return m_menuBar;
}

QPopupMenu* MainWindowBase::menuFile() {
  return m_menuFile;
}

QPopupMenu* MainWindowBase::menuDebugger() {
  return m_menuDebugger;
}


QPopupMenu* MainWindowBase::menuEditor() {
  return m_menuEditor;
}

QAction* MainWindowBase::editUndoAction() {
  return m_editUndoAction;
}
QAction* MainWindowBase::editRedoAction() {
  return m_editRedoAction;
}
QAction* MainWindowBase::editCutAction() {
  return m_editCutAction;
}
QAction* MainWindowBase::editCopyAction() {
  return m_editCopyAction;
}
QAction* MainWindowBase::editPasteAction() {
  return m_editPasteAction;
}


#include "mainwindowbase.moc"
