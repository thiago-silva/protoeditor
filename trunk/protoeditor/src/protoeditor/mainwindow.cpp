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
#include "sitesettings.h"
#include "debuggermanager.h"
#include "configdlg.h"
#include "protoeditorsettings.h"

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
#include <ktexteditor/view.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <qvaluelist.h>


#include <ktextedit.h>
//#include <kconfigdialog.h>


/*
#include <ktexteditor/document.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/view.h>
#include <ktexteditor/editinterface.h>
*/


#include <qlayout.h>
#include <qsplitter.h>


MainWindow::MainWindow(QWidget* parent, const char* name, WFlags fl)
//     : KMainWindow(parent, name, fl),  m_debuggerSettings(0), m_browserSettings(0)
  : KParts::MainWindow(parent, name, fl),  m_debuggerSettings(0), m_browserSettings(0)
{
  if(!name) { setName("MainWindow"); }

  setupStatusBar();

  m_debugger_manager = new DebuggerManager(this);
    
  setupActions();
  
  createWidgets();

  createGUI(0);

  resize( QSize(633, 533).expandedTo(minimumSizeHint()) );
  clearWState(WState_Polished);

  m_debugger_manager->init();

  connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(slotClose()));

  connect(ProtoeditorSettings::self(), SIGNAL(sigSettingsChanged()),
    this, SLOT(slotSettingsChanged()));

  connect(m_siteAction, SIGNAL(activated(const QString&)),
    ProtoeditorSettings::self(), SLOT(slotCurrentSiteChanged(const QString&)));

  loadSites();

  stateChanged("init");
}

void MainWindow::loadSites()
{
  QStringList strsites;

  QValueList<SiteSettings*> sitesList = ProtoeditorSettings::self()->siteSettingsList();
  QValueList<SiteSettings*>::iterator it;

  for(it = sitesList.begin(); it != sitesList.end(); ++it) {
    strsites << (*it)->name();
  }
  m_siteAction->setItems(strsites);

  if(strsites.count()) {

    m_siteAction->setCurrentItem(0);
    //note 1: KSelectAction doesn't emit activated() when calling setCurrentItem()

    ProtoeditorSettings::self()->slotCurrentSiteChanged(m_siteAction->currentText());
  }

  //note 2: KSelectAction doesn't updates its combo width, so we have to call this
  m_siteAction->setComboWidth(150);
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
  m_actionRecent->loadEntries(kapp->config());//,"Recent Files");

  KStdAction::close(this, SLOT(slotCloseFile()), actionCollection());

  (void)new KAction(i18n("Close All"), 0, this, SLOT(slotCloseAllFiles()), actionCollection(), "file_close_all");
  
  KStdAction::quit(this, SLOT(slotQuit()), actionCollection());

  KStdAction::keyBindings(this, SLOT(slotEditKeys()), actionCollection());
  KStdAction::configureToolbars(this, SLOT(slotEditToolbars()), actionCollection());
  KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection(), "settings_protoeditor");

  m_siteAction = new KSelectAction("Site", 0, actionCollection(), "site");
//   (void)new KAction(i18n("&Run"), "gear", "F9", m_debugger_manager,
//                     SLOT(slotDebugRun()), actionCollection(), "script_run");

  (void)new KAction(i18n("Start Debug"), "dbgstart", "F5", m_debugger_manager,
                    SLOT(slotDebugRun()), actionCollection(), "debug_start");

  (void)new KAction(i18n("Stop Debug"), "stop", "ESC", m_debugger_manager,
                    SLOT(slotDebugStop()), actionCollection(), "debug_stop");

  (void)new KAction(i18n("Step Over"), "dbgnext", "F6", m_debugger_manager,
                    SLOT(slotDebugStepOver()), actionCollection(), "debug_step_over");

  (void)new KAction(i18n("Step Into"), "dbgstep", "F7", m_debugger_manager,
                    SLOT(slotDebugStepInto()), actionCollection(), "debug_step_into");

  (void)new KAction(i18n("Step Out"), "dbgstepout", "F8", m_debugger_manager,
                    SLOT(slotDebugStepOut()), actionCollection(), "debug_step_out");

  (void)new KAction(i18n("Toggle Breakpoint"), "activebreakpoint", "Alt+B", m_debugger_manager,
                    SLOT(slotDebugToggleBp()), actionCollection(), "debug_toggle_bp");
  
  setStandardToolBarMenuEnabled(true);
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

  QTabWidget* tabDebug = new QTabWidget(splitter);
  splitter->setCollapsible(tabDebug, false);
  tabDebug->setGeometry(0,0,0,height()/15);

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

  /*
  KTextEditor::Document* doc = KTextEditor::EditorChooser::createDocument(
      0L, "KTextEditor::Document");
  //doc->setReadWrite(false);
  m_edOutput = dynamic_cast<KTextEditor::EditInterface*>(doc);
  m_edOutput->setText("oioi");
  outputTabLayout->addWidget(doc->createView(outputTab));
  */

  tabDebug->insertTab(outputTab, QString("Output"));

}

MainWindow::~MainWindow()
{
  /* TODO: delete all widgets? */
  delete m_debugger_manager;
}

void MainWindow::slotSettingsChanged()
{
  loadSites();
}

void MainWindow::openFile() {
  slotOpenFile();
}

void MainWindow::slotOpenFile()
{

  QStringList strList =
    KFileDialog::getOpenFileNames(
      ":protoeditor", "*.php", this);

  if(strList.count()) {
    for(QStringList::Iterator it = strList.begin(); it != strList.end(); ++it ) {
      openFile(*it);
    }
  }
}


void MainWindow::openFile(const KURL& url)
{
  KFileItem file(KFileItem::Unknown, KFileItem::Unknown, url);
  if(file.isReadable()) {
    m_tabEditor->openDocument(url);
    m_actionRecent->addURL(url);

  } else {
    m_actionRecent->removeURL(url);
    showSorry(url.prettyURL() + " is unreadable.");
  }
}


void MainWindow::slotFileRecent(const KURL& url)
{
  openFile(url);
}

void MainWindow::slotCloseFile()
{
  m_tabEditor->closeCurrentDocument();
}

void MainWindow::slotCloseAllFiles()
{
  m_tabEditor->closeAllDocuments();
}

void MainWindow::slotSaveFile()
{
  if(!m_tabEditor->saveCurrentFile()) {
    showSorry("Unable to save file");
  }
}

void MainWindow::slotSaveFileAs()
{
  KURL url = KFileDialog::getSaveURL(":protoeditor", "*.php", this);

  if(!url.isEmpty()) {
    if(!m_tabEditor->saveCurrentFileAs(url)) {
      showSorry("Unable to save file");
    }
  }
}

// bool MainWindow::close()
// {
//   m_tabEditor->closeAllDocuments();
//   m_actionRecent->saveEntries(kapp->config());
//   kapp->config()->sync();
// }

void MainWindow::slotClose()
{
  m_actionRecent->saveEntries(kapp->config());
  kapp->config()->sync();
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
    KTextEditor::View* view  = m_tabEditor->currentView();
    if(view) {
      dlg.insert(view->actionCollection());
    }  
  }

  dlg.configure();
}

void MainWindow::actionStateChanged(const QString& str)
{
  stateChanged(str);
}

void MainWindow::slotEditToolbars()
{
  KEditToolbar dlg(guiFactory());
  if (dlg.exec()) {
    //setupGUI();
    applyMainWindowSettings( KGlobal::config(), autoSaveGroup() );
  }
}

void MainWindow::slotShowSettings()
{
  ConfigDlg::showDialog();
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
