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

#include "mainwindow.h"
#include "editortabwidget.h"
#include "variableslistview.h"
#include "watchlistview.h"
#include "loglistview.h"
#include "debuggercombostack.h"
#include "breakpointlistview.h"

#include "debuggermanager.h"

#include "debuggersettings.h"
#include "debuggersettingswidget.h"

#include <kapplication.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <ktextedit.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>
#include <kconfigdialog.h>
#include <ktexteditor/view.h>
#include <kfiledialog.h>
#include <kfileitem.h>

#include <qlayout.h>
#include <qsplitter.h>


MainWindow::MainWindow(QWidget* parent, const char* name, WFlags fl)
    : KMainWindow(parent, name, fl),  m_debuggerSettings(0)
{
  if(!name) { setName("MainWindow"); }

  m_debugger_manager = new DebuggerManager(this);

  setupStatusBar();

  createWidgets();

  setStandardToolBarMenuEnabled(true);

  setupActions();

  createGUI();

  resize( QSize(633, 533).expandedTo(minimumSizeHint()) );
  clearWState(WState_Polished);

  m_tabEditor->init();

  m_debugger_manager->init();

  connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(slotClose()));
}


/* Thanks to KEdit author, we copy and paste :-) */
void MainWindow::setupStatusBar()
{
  m_statusBar = new KStatusBar(this);
  /*
    statusBar()->insertItem("", ID_GENERAL, 10 );
    statusBar()->insertFixedItem( i18n("OVR"), ID_INS_OVR );
    statusBar()->insertFixedItem( i18n("Line:000000 Col: 000"), ID_LINE_COLUMN );

    statusBar()->setItemAlignment( ID_GENERAL, AlignLeft|AlignVCenter );
    statusBar()->setItemAlignment( ID_LINE_COLUMN, AlignLeft|AlignVCenter );
    statusBar()->setItemAlignment( ID_INS_OVR, AlignLeft|AlignVCenter );

    statusBar()->changeItem( i18n("Line: 1 Col: 1"), ID_LINE_COLUMN );
    statusBar()->changeItem( i18n("INS"), ID_INS_OVR );
  */
}

void MainWindow::setupActions()
{
  //NOTE: there are A LOT of std actions that can be used.

  //file menu
  KStdAction::open(this, SLOT(slotOpenFile()), actionCollection());

  m_actionRecent = KStdAction::openRecent(this, SLOT(slotFileRecent(const KURL&)), actionCollection());
  m_actionRecent->loadEntries(kapp->config(),"Recent Files");

  KStdAction::close(this, SLOT(slotCloseFile()), actionCollection());
  KStdAction::save(this, SLOT(slotSaveFile()), actionCollection());
  KStdAction::saveAs(this, SLOT(slotSaveFileAs()), actionCollection());
  KStdAction::quit(this, SLOT(slotQuit()), actionCollection());


  //edit menu
  KStdAction::undo(m_tabEditor,      SLOT(slotUndo()), actionCollection());
  KStdAction::redo(m_tabEditor,      SLOT(slotRedo()), actionCollection());
  KStdAction::cut(m_tabEditor,       SLOT(slotCut()), actionCollection());
  KStdAction::copy(m_tabEditor,      SLOT(slotCopy()), actionCollection());
  KStdAction::paste(m_tabEditor,     SLOT(slotPaste()), actionCollection());
  KStdAction::selectAll(m_tabEditor, SLOT(slotSelectAll()), actionCollection());

  KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection(), "settings_protoeditor");

  KStdAction::keyBindings(this, SLOT(slotEditKeys()), actionCollection());
  KStdAction::configureToolbars(this, SLOT(slotEditToolbars()), actionCollection());

  (void)new KAction(i18n("Configure &Editor..."), 0, m_tabEditor, SLOT(slotConfigEditor()), actionCollection(), "settings_editor");

  // DEBUGGER Actions
  (void)new KAction(i18n("&Start Session"), "connect_creating", "F9", m_debugger_manager,
    SLOT(slotDebugStartSession()), actionCollection(), "debug_start_session");

  (void)new KAction(i18n("&End Session"), "connect_no", "F10", m_debugger_manager,
    SLOT(slotDebugEndSession()), actionCollection(), "debug_end_session");

  (void)new KAction(i18n("&Run"), "dbgrun", "F5", m_debugger_manager,
    SLOT(slotDebugRun()), actionCollection(), "debug_run");

  (void)new KAction(i18n("&Stop"), "stop", "ESC", m_debugger_manager,
    SLOT(slotDebugStop()), actionCollection(), "debug_stop");

  (void)new KAction(i18n("Step &Over"), "dbgnext", "F6", m_debugger_manager,
    SLOT(slotDebugStepOver()), actionCollection(), "debug_step_over");

  (void)new KAction(i18n("Step &Into"), "dbgstep", "F7", m_debugger_manager,
    SLOT(slotDebugStepInto()), actionCollection(), "debug_step_into");

  (void)new KAction(i18n("Step O&ut"), "dbgstepout", "F8", m_debugger_manager,
    SLOT(slotDebugStepOut()), actionCollection(), "debug_step_out");

  (void)new KAction(i18n("&Toggle Breakpoint"), "activebreakpoint", "", this,
    SLOT(slotDebugToggleBp()), actionCollection(), "debug_toggle_bp");
}

void MainWindow::createWidgets()
{
  setCentralWidget(new QWidget(this));
  QVBoxLayout* mainFormBaseLayout = new QVBoxLayout(centralWidget(), 1, 1);

  QSplitter* splitter = new QSplitter(centralWidget());
  splitter->setOrientation( Qt::Vertical  );
  mainFormBaseLayout->addWidget(splitter);

  m_tabEditor = new EditorTabWidget(splitter, this);
  splitter->setCollapsible(m_tabEditor, false);
  splitter->setOpaqueResize(true);
  //mainFormBaseLayout->addWidget(m_tabEditor);

  //QTabWidget* tabDebug = new QTabWidget(centralWidget());
  QTabWidget* tabDebug = new QTabWidget(splitter);
  splitter->setCollapsible(tabDebug, false);
  //tabDebug->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, tabDebug->sizePolicy().hasHeightForWidth()));
  tabDebug->setGeometry(0,0,0,height()/15);
  //mainFormBaseLayout->addWidget(tabDebug);

  QWidget* globalVarTab = new QWidget(tabDebug);
  QVBoxLayout* globalVarTabLayout = new QVBoxLayout(globalVarTab, 1, 1);

  m_globaVarList = new VariablesListView(globalVarTab);
  globalVarTabLayout->addWidget(m_globaVarList);
  tabDebug->insertTab(globalVarTab, QString("Global"));

  QWidget* tabStack = new QWidget(tabDebug);
  QVBoxLayout* varTabLayout = new QVBoxLayout(tabStack, 1, 1);
  QHBoxLayout* stackComboLayout = new QHBoxLayout(0, 6, 6);

  QLabel* stackLabel = new QLabel(tabStack);
  stackLabel->setText(("Stack:"));
  stackLabel->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, stackLabel->sizePolicy().hasHeightForWidth()));
  stackComboLayout->addWidget(stackLabel);

  m_stackCombo = new DebuggerComboStack(tabStack);
  stackComboLayout->addWidget(m_stackCombo);
  varTabLayout->addLayout(stackComboLayout);

  m_localVarList= new VariablesListView(tabStack);
  varTabLayout->addWidget(m_localVarList);
  tabDebug->insertTab(tabStack, QString("Local"));

  QWidget* tabWatch = new QWidget(tabDebug);
  QVBoxLayout* watchTabLayout = new QVBoxLayout(tabWatch, 1, 1);

  QHBoxLayout* addWatchLayout = new QHBoxLayout(0, 6, 6);

  QLabel* addLabel = new QLabel(tabWatch);
  addLabel->setText(("Watch:"));
  addWatchLayout->addWidget(addLabel);

  m_edAddWatch = new KLineEdit(tabWatch);
  m_edAddWatch->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_edAddWatch->sizePolicy().hasHeightForWidth()));
  addWatchLayout->addWidget(m_edAddWatch);

  m_btAddWatch = new KPushButton(tabWatch);
  m_btAddWatch->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_btAddWatch->sizePolicy().hasHeightForWidth()));
  m_btAddWatch->setText(("Add"));
  addWatchLayout->addWidget(m_btAddWatch);

  QSpacerItem* spacer = new QSpacerItem(430, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  addWatchLayout->addItem(spacer);
  watchTabLayout->addLayout(addWatchLayout);

  m_watchList = new WatchListView(tabWatch);
  watchTabLayout->addWidget(m_watchList);
  tabDebug->insertTab(tabWatch, QString("Watch"));

  QWidget* breakpointTab = new QWidget(tabDebug);
  QVBoxLayout* breakpointTabLayout = new QVBoxLayout(breakpointTab, 1, 1);

  m_breakpointList = new BreakpointListView(breakpointTab);
  breakpointTabLayout->addWidget(m_breakpointList);
  tabDebug->insertTab(breakpointTab, QString("Breakpoints"));

  QWidget* logTab = new QWidget(tabDebug);
  QVBoxLayout* logTabLayout = new QVBoxLayout(logTab, 1, 1);

  m_logListView = new LogListView(logTab);
  logTabLayout->addWidget(m_logListView);
  tabDebug->insertTab(logTab, QString("Messages"));

  QWidget* outputTab = new QWidget(tabDebug);
  QVBoxLayout* outputTabLayout = new QVBoxLayout(outputTab, 1, 1);

  m_edOutput = new KTextEdit(outputTab);
  outputTabLayout->addWidget(m_edOutput);
  m_edOutput->setReadOnly(true);
  m_edOutput->setTextFormat(Qt::PlainText);
  m_edOutput->setPaper( QBrush(QColor("white")));

  //m_edOutput->setTextFormat(Qt::LogText);
  tabDebug->insertTab(outputTab, QString("Output"));

  /*
  connect(m_tabEditor, SIGNAL(sigHasNoFiles()), this, SLOT(slotHasNoFiles()));
  connect(m_tabEditor, SIGNAL(sigHasFiles()), this, SLOT(slotHasFiles()));
  connect(m_tabEditor, SIGNAL(sigHasNoUndo()), this, SLOT(slotHasNoUndo()));
  connect(m_tabEditor, SIGNAL(sigHasUndo()), this, SLOT(slotHasUndo()));
  connect(m_tabEditor, SIGNAL(sigHasNoRedo()), this, SLOT(slotHasNoRedo()));
  connect(m_tabEditor, SIGNAL(sigHasRedo()), this, SLOT(slotHasRedo()));
  */
}

/*
*  Destroys the object and frees any allocated resources
*/
MainWindow::~MainWindow()
{
  /* TODO: delete all widgets? */
}

void MainWindow::slotOpenFile()
{

  QStringList strList =
    KFileDialog::getOpenFileNames(
      ":protoeditor_openphp", "*.php", this);

  if(strList.count()) {
    for(QStringList::Iterator it = strList.begin(); it != strList.end(); ++it ) {
      openFile(*it);
    }
  }
}


void MainWindow::openFile(const KURL& url) {
  KFileItem file(KFileItem::Unknown, KFileItem::Unknown, url);
  if(file.isReadable()) {
    m_tabEditor->addDocument(url);
    m_actionRecent->addURL(url);

    //the active tab will be a brand new file with no history, so clear the
    //undo/redo
    //slotHasNoRedo();
    //slotHasNoUndo();
  } else {
    m_actionRecent->removeURL(url);
    showSorry(url.prettyURL() + " is unreadable.");
  }
}


void MainWindow::slotFileRecent(const KURL& url) {
  openFile(url);
}

void MainWindow::slotCloseFile()
{
  m_tabEditor->closeCurrentDocument();
}

void MainWindow::slotSaveFile()
{
  if(!m_tabEditor->saveCurrentFile()) {
    showSorry("Unable to save file");
  }
}

void MainWindow::slotSaveFileAs()
{
  KURL url = KFileDialog::getSaveURL(":protoeditor_openphp", "*.php", this);

  if(!url.isEmpty()) {
    if(!m_tabEditor->saveCurrentFileAs(url)) {
      showSorry("Unable to save file");
    }
  }
}

void MainWindow::slotClose() {
  //tabEditor()->terminate();
  m_actionRecent->saveEntries(kapp->config(), "Recent Files");
}

void MainWindow::slotQuit()
{
  close();
}

void MainWindow::showError(const QString& msg) const
{
  KMessageBox::error(0, msg);
}

void MainWindow::slotEditKeys()
{
  KKeyDialog dlg;
  dlg.insert(actionCollection());

  if(m_tabEditor->count() != 0) {
    KTextEditor::View* view  = m_tabEditor->anyView();
    dlg.insert(view->actionCollection());
  }

  dlg.configure();
}

void MainWindow::slotEditToolbars()
{
  KEditToolbar dlg(actionCollection());
  if (dlg.exec())
    createGUI();
}

/*
void MainWindow::slotHasNoFiles()
{
  actionCollection()->action("file_close")->setEnabled(false);
  actionCollection()->action("file_save")->setEnabled(false);
  actionCollection()->action("file_save_as")->setEnabled(false);

  actionCollection()->action("edit_undo")->setEnabled(false);
  actionCollection()->action("edit_redo")->setEnabled(false);
  actionCollection()->action("edit_cut")->setEnabled(false);
  actionCollection()->action("edit_copy")->setEnabled(false);
  actionCollection()->action("edit_paste")->setEnabled(false);
  actionCollection()->action("edit_select_all")->setEnabled(false);

  actionCollection()->action("settings_editor")->setEnabled(false);
}


void MainWindow::slotHasFiles()
{
  actionCollection()->action("file_close")->setEnabled(true);
  actionCollection()->action("file_save")->setEnabled(true);
  actionCollection()->action("file_save_as")->setEnabled(true);

  actionCollection()->action("edit_undo")->setEnabled(true);
  actionCollection()->action("edit_redo")->setEnabled(true);
  actionCollection()->action("edit_cut")->setEnabled(true);
  actionCollection()->action("edit_copy")->setEnabled(true);
  actionCollection()->action("edit_paste")->setEnabled(true);
  actionCollection()->action("edit_select_all")->setEnabled(true);

  actionCollection()->action("settings_editor")->setEnabled(true);
}
*/

/*
void MainWindow::slotHasNoUndo()
{
  actionCollection()->action("edit_undo")->setEnabled(false);
}

void MainWindow::slotHasUndo()
{
  actionCollection()->action("edit_undo")->setEnabled(true);
}

void MainWindow::slotHasNoRedo()
{
  actionCollection()->action("edit_redo")->setEnabled(false);
}

void MainWindow::slotHasRedo()
{
  actionCollection()->action("edit_redo")->setEnabled(true);
}
*/
void MainWindow::slotShowSettings()
{
  if(KConfigDialog::showDialog("settings"))
    return;

  KConfigDialog* dialog = new KConfigDialog(this, "settings", DebuggerSettings::self());

  initDebuggerSettings();
  dialog->addPage( m_debuggerSettings, i18n("Debugger"), "debugger" );
  //dialog->addPage( m_debuggerSettings, i18n("Debugger"), "bug" );


  connect( dialog, SIGNAL(settingsChanged()),
           m_debugger_manager, SLOT(slotConfigurationChanged()) );

  dialog->show();
}

void MainWindow::initDebuggerSettings()
{
  if(!m_debuggerSettings) {
    m_debuggerSettings = new DebuggerSettingsWidget(0, "Debugger");
  }
}

void MainWindow::slotDebugToggleBp() {
}

void MainWindow::showSorry(const QString& msg) const
{
  KMessageBox::sorry(0, msg);
}

EditorTabWidget* MainWindow::tabEditor()
{
  return m_tabEditor;
}

DebuggerComboStack* MainWindow::stackCombo()
{
  return m_stackCombo;
}

VariablesListView*  MainWindow::globalVarList()
{
  return m_globaVarList;
}

VariablesListView*  MainWindow::localVarList()
{
  return m_localVarList;
}

KLineEdit* MainWindow::edAddWatch()
{
  return m_edAddWatch;
}

KPushButton* MainWindow::btAddWatch()
{
  return m_btAddWatch;
}

WatchListView*  MainWindow::watchList()
{
  return m_watchList;
}

BreakpointListView* MainWindow::breakpointListView()
{
  return m_breakpointList;
}

LogListView* MainWindow::logListView()
{
  return m_logListView;
}

KTextEdit* MainWindow::edOutput()
{
  return m_edOutput;
}

#include "mainwindow.moc"
