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

#include <kapplication.h>
#include <kconfig.h>
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
#include <ktexteditor/view.h>


#include <qlayout.h>
#include <qfiledialog.h>
#include <qsplitter.h>


/*
#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>

#include <qlabel.h>
#include <qcombobox.h>
#include <qheader.h>
#include <klistview.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <klocale.h>
*/

MainWindow::MainWindow(QWidget* parent, const char* name, WFlags fl)
    : KMainWindow(parent, name, fl), m_currentOpenPath("/")
{
  if(!name) { setName("MainWindow"); }

  setupConfiguration();

  setupStatusBar();

  createWidgets();

  setupActions();

  setStandardToolBarMenuEnabled(true);

  createGUI();
  //??? need this?
  //m_tabEditor->setMainWindow(this);

  //m_debugger_manager = new DebuggerManager(this);

  resize( QSize(633, 533).expandedTo(minimumSizeHint()) );
  clearWState(WState_Polished);

  slotHasNoFiles();
}


void MainWindow::setupConfiguration()
{

  KConfig* config = KApplication::kApplication()->config();
  /*
  if(config->hasGroup ("Protoeditor")) {
    config->setGroup("Protoeditor");

    if(config->hasKey("xpos")  &&
       config->hasKey("ypos")  &&
       config->hasKey("width") &&
       config->hasKey("height")) {

      setGeometry(config->readNumEntry("xpos"),
                  config->readNumEntry("ypos"),
                  config->readNumEntry("width"),
                  config->readNumEntry("height"));
    }
  }
  */

  config->setGroup("Protoeditor");
  if(config->hasKey("CurrentFilePath")) {
    m_currentOpenPath = config->readEntry("CurrentFilePath");
  }
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

  KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection());

  KStdAction::keyBindings(this, SLOT(slotEditKeys()), actionCollection());
  KStdAction::configureToolbars(this, SLOT(slotEditToolbars()), actionCollection());

  (void)new KAction(i18n("Configure &Editor..."), 0, m_tabEditor, SLOT(slotConfigEditor()), actionCollection(), "settings_editor");
  //(void)new KAction(i18n("Configure &Debugger..."), 0, m_tabEditor, SLOT(slotConfigEditor()), actionCollection(), "config_debugger");



  /*
  KStdAction::aboutApp(this, SLOT(slotAbout()), actionCollection());
  KStdAction::aboutKDE(this, SLOT(slotAboutKDE()), actionCollection());
  */
  /* DEBUGGER Actions
  (void)new KAction(i18n("&Insert File..."), 0, this, SLOT(file_insert()),
                    actionCollection(), "insert_file");
  (void)new KAction(i18n("In&sert Date"), 0, this, SLOT(insertDate()),
                    actionCollection(), "insert_date");
  (void)new KAction(i18n("Cl&ean Spaces"), 0, this, SLOT(clean_space()),
                    actionCollection(), "clean_spaces");
  */
}

void MainWindow::createWidgets()
{
  setCentralWidget(new QWidget(this));
  QVBoxLayout* mainFormBaseLayout = new QVBoxLayout(centralWidget(), 1, 1);

  QSplitter* splitter = new QSplitter(centralWidget());
  splitter->setOrientation( Qt::Vertical  );
  mainFormBaseLayout->addWidget(splitter);

  //m_tabEditor = new EditorTabWidget(centralWidget());
  m_tabEditor = new EditorTabWidget(splitter);
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
  tabDebug->insertTab(outputTab, QString("Output"));

  connect(m_tabEditor, SIGNAL(sigHasNoFiles()), this, SLOT(slotHasNoFiles()));
  connect(m_tabEditor, SIGNAL(sigHasFiles()), this, SLOT(slotHasFiles()));
  connect(m_tabEditor, SIGNAL(sigHasNoUndo()), this, SLOT(slotHasNoUndo()));
  connect(m_tabEditor, SIGNAL(sigHasUndo()), this, SLOT(slotHasUndo()));
  connect(m_tabEditor, SIGNAL(sigHasNoRedo()), this, SLOT(slotHasNoRedo()));
  connect(m_tabEditor, SIGNAL(sigHasRedo()), this, SLOT(slotHasRedo()));
  //connect(m_tabEditor, SIGNAL(sigTabChanged(int)), this, SLOT(slotTabChanged()));

}

/*
*  Destroys the object and frees any allocated resources
*/
MainWindow::~MainWindow()
{
  //saveGeometry();
  saveCurrentPath();
  /* TODO: delete all widgets? */
}

void MainWindow::saveCurrentPath()
{
  KConfig* config = KApplication::kApplication()->config();
  config->setGroup("Protoeditor");
  config->writeEntry("CurrentFilePath", m_currentOpenPath);
  config->sync();
}

/*
void MainWindow::saveGeometry()
{
  KConfig* config = KApplication::kApplication()->config();
  config->setGroup("Protoeditor");
  config->writeEntry("xpos", x());
  config->writeEntry("ypos", y());
  config->writeEntry("width", width());
  config->writeEntry("height", height());
  config->sync();
}
*/

void MainWindow::slotOpenFile()
{
  QString filePath = QFileDialog::getOpenFileName(m_currentOpenPath, "PHP (*.php)",
                                                  this, "open file dialog", "Choose a file" );

  if(!filePath.isEmpty()) {
    m_currentOpenPath = filePath.mid(0, filePath.findRev('/', -1));
    //TODO: set addDocument to receive KURL
    m_tabEditor->addDocument(filePath);
    slotHasNoRedo();
    slotHasNoUndo();
  }
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
{}

void MainWindow::slotQuit()
{
  //tabEditor()->terminate();
  close();
}

void MainWindow::slotShowSettings()
{}

void MainWindow::showError(const QString& msg) const
{
  KMessageBox::error(0, msg);
}

void MainWindow::slotEditKeys()
{
  KKeyDialog dlg;
  dlg.insert(actionCollection());

  KTextEditor::View* view = 0;

  if(m_tabEditor->count() != 0) {
    KTextEditor::View* view  = m_tabEditor->anyView();
    dlg.insert(view->actionCollection());
  }

  dlg.configure();
}

void MainWindow::slotEditToolbars()
{
  KEditToolbar *dlg = new KEditToolbar(actionCollection());

  if (dlg->exec()) {
    /*
    KParts::GUIActivateEvent ev1( false );
    QApplication::sendEvent( m_view, &ev1 );
    guiFactory()->removeClient( m_view );
    createShellGUI( false );
    createShellGUI( true );
    guiFactory()->addClient( m_view );
    KParts::GUIActivateEvent ev2( true );
    QApplication::sendEvent( m_view, &ev2 );
    */
  }

  delete dlg;
}

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
