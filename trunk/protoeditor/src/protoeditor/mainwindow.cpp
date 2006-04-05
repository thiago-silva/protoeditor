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

#include "mainwindow.h"
#include "editortabwidget.h"
#include "variableslistview.h"
#include "watchlistview.h"
#include "messagelistview.h"
#include "debuggercombostack.h"
#include "breakpointlistview.h"
#include "sitesettings.h"
#include "debuggermanager.h"
#include "configdlg.h"
#include "protoeditorsettings.h"
#include "phpsettings.h"

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
#include <kcombobox.h>
#include <qvaluelist.h>

#include <kdialogbase.h>
#include <kiconloader.h>
#include <ktextedit.h>
#include <kencodingfiledialog.h>
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

  createWidgets();

  setupActions();

  createGUI(0);

  resize( QSize(633, 533).expandedTo(minimumSizeHint()) );
  clearWState(WState_Polished);

  m_debugger_manager->init();

  connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(slotClose()));

  connect(ProtoeditorSettings::self(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  loadSites();

  stateChanged("init");

  m_cbArguments->insertStringList(ProtoeditorSettings::self()->argumentsHistory());
  m_cbArguments->clearEdit();
}

void MainWindow::loadSites()
{
  QString currentSiteName = m_siteAction->currentText();

  QStringList strsites;

  strsites << ProtoeditorSettings::LocalSiteName;

  QValueList<SiteSettings*> sitesList = ProtoeditorSettings::self()->siteSettingsList();
  QValueList<SiteSettings*>::iterator it;

  int i = 0;
  int curr = 0;
  for(it = sitesList.begin(); it != sitesList.end(); ++it)
  {
    strsites << (*it)->name();
    if((*it)->name() == currentSiteName) {
      curr = i+1; //LocalSiteName already inserted
    }
    i++;
  }
    
  m_siteAction->setItems(strsites);
  m_siteAction->setCurrentItem(curr);
    
  ProtoeditorSettings::self()->slotCurrentSiteChanged(m_siteAction->currentText());  
//   slotCurrentSiteChanged(m_siteAction->currentText());
}

void MainWindow::setupStatusBar()
{
  m_statusBar = new KStatusBar(this);

  //first item: debug led
  m_lbLed = new QLabel(this);
  m_lbLed->setPixmap(QPixmap(UserIcon("indicator_off")));
  m_lbLed->setAlignment( Qt::AlignCenter );
  m_lbLed->setSizePolicy(QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ));
  m_statusBar->addWidget(m_lbLed);

  //second item: debugger name
  m_lbDebugName = new QLabel(this);
  m_lbDebugName->setMinimumSize( 40, 0 );
  //m_lbDebugMsg->setSizePolicy(QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ));
  m_statusBar->addWidget(m_lbDebugName, 0 /*stretch*/, false );
  
  //third item: debug msgs
  m_lbDebugMsg = new QLabel(this);
  m_lbDebugMsg->setMinimumSize( 0, 0 );
  m_lbDebugMsg->setSizePolicy(QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ));
  m_statusBar->addWidget(m_lbDebugMsg, 1 /*stretch*/, false );

  //fourth item: editor msgs
  m_lbEditorMsg = new QLabel(this);
  m_lbEditorMsg->setMinimumSize( 230, 0 );
  m_lbEditorMsg->setSizePolicy(QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ));
  m_lbEditorMsg->setAlignment( Qt::AlignCenter );
  m_statusBar->addWidget(m_lbEditorMsg, 1 /*stretch*/, false );

}

void MainWindow::setupActions()
{
  //file menu
  KStdAction::openNew(this, SLOT(slotNewFile()), actionCollection());

  KStdAction::open(this, SLOT(slotOpenFile()), actionCollection());

  KStdAction::save(this, SLOT(slotSaveCurrentFile()), actionCollection());
  KStdAction::saveAs(this, SLOT(slotSaveCurrentFileAs()), actionCollection());

  m_actionRecent = KStdAction::openRecent(this, SLOT(slotFileRecent(const KURL&)), actionCollection());
  m_actionRecent->loadEntries(kapp->config());//,"Recent Files");

  KStdAction::close(this, SLOT(slotCloseFile()), actionCollection());

  (void)new KAction(i18n("Close All"), 0, this, SLOT(slotCloseAllFiles()), actionCollection(), "file_close_all");

  KStdAction::quit(this, SLOT(slotQuit()), actionCollection());

  KStdAction::keyBindings(this, SLOT(slotEditKeys()), actionCollection());
  KStdAction::configureToolbars(this, SLOT(slotEditToolbars()), actionCollection());

  KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection(), "settings_protoeditor");

  m_siteAction     = new KSelectAction("Site", 0, actionCollection(), "site_selection");
  m_siteAction->setComboWidth(150);

  connect(m_siteAction, SIGNAL(activated(const QString&)),
      ProtoeditorSettings::self(), SLOT(slotCurrentSiteChanged(const QString&)));

//   connect(m_siteAction, SIGNAL(activated(const QString&)),
//       this, SLOT(slotCurrentSiteChanged(const QString&)));
  

  m_activeScriptAction = new KToggleAction("Use Current Script", "attach", 0, actionCollection(), "use_current_script");

  (void)new KAction(i18n("Run in Console"), "gear", "F9", this,
                    SLOT(slotScriptRun()), actionCollection(), "script_run");

  (void)new KAction(i18n("Start Debug"), "dbgstart", "F5", this,
                    SLOT(slotDebugStart()), actionCollection(), "debug_start");

  (void)new KAction(i18n("Stop Debug"), "stop", "Escape", m_debugger_manager,
                    SLOT(slotDebugStop()), actionCollection(), "debug_stop");

  (void)new KAction(i18n("Step Into"), "dbgstep", "F6", m_debugger_manager,
                    SLOT(slotDebugStepInto()), actionCollection(), "debug_step_into");

  (void)new KAction(i18n("Step Over"), "dbgnext", "F7", m_debugger_manager,
                    SLOT(slotDebugStepOver()), actionCollection(), "debug_step_over");

  (void)new KAction(i18n("Step Out"), "dbgstepout", "F8", m_debugger_manager,
                    SLOT(slotDebugStepOut()), actionCollection(), "debug_step_out");

/* (void)new KAction(i18n("Profile (DBG only)"), "math_sum", "Alt+P", 
      m_debugger_manager, SLOT(slotProfile()), actionCollection(), "script_profile");*/
  
  (void)new KAction(i18n("Toggle Breakpoint"), "activebreakpoint", "Alt+B", m_debugger_manager,
                    SLOT(slotDebugToggleBp()), actionCollection(), "debug_toggle_bp");

  (void)new KAction(i18n("Add Watch"), "math_brace", 0, m_tabEditor,
                    SLOT(slotAddWatch()), actionCollection(), "editor_add_watch");


  //arguments tool bar
  m_cbArguments = new KHistoryCombo(true, this);

  KWidgetAction* comboAction = new KWidgetAction( m_cbArguments, "Argument Bar ", 0,
                  0, 0, actionCollection(), "argument_combobox" );
  comboAction->setShortcutConfigurable(false);
  comboAction->setAutoSized(true);


  (void)new KAction("Clear Arguments", "clear_left", 0, m_cbArguments,
                    SLOT(clearEdit()), actionCollection(), "argument_clear");

  (void)new KWidgetAction(new QLabel("Arguments: ", this), "Arguments ", "Alt+a", 
      this, SLOT(slotFocusArgumentBar()), actionCollection(), "argument_label" );
  

  //finish!
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

  m_messageListView = new MessageListView(logTab);
  logTabLayout->addWidget(m_messageListView);
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
  //removes the current text (wich might not have been used in debug/execution)
  m_cbArguments->clearEdit();

  ProtoeditorSettings::self()->setArgumentsHistory(m_cbArguments->historyItems());

  ProtoeditorSettings::self()->writeConfig(true);

  delete m_debugger_manager;
}

bool MainWindow::useCurrentScript()
{
  return m_activeScriptAction->isChecked();
}

void MainWindow::slotSettingsChanged()
{
  loadSites();
}

void MainWindow::openFile()
{
  slotOpenFile();
}

void MainWindow::slotNewFile()
{
  m_tabEditor->createNew();
}

void MainWindow::slotOpenFile()
{
  //Show KFileDialog on the current Site's "local base dir" or
  //use ::protoeditor to retrieve the last folder used

  SiteSettings* currentSite = ProtoeditorSettings::self()->currentSiteSettings();
  QString location;
  if(currentSite)
  {
    location = currentSite->localBaseDir().path();
  }
  else
  {
    location = ":protoeditor";
  }

  //note: the filter must be the same as SiteSettingsDialog::SiteSettingsDialog default file
  QStringList strList =
    KFileDialog::getOpenFileNames(
      location, "*.php| PHP Scripts\n*|All Files", this);

  if(strList.count())
  {
    for(QStringList::Iterator it = strList.begin(); it != strList.end(); ++it )
    {
      openFile(KURL::fromPathOrURL(*it));
    }
  }
}

void MainWindow::openFile(const KURL& url)
{
  KFileItem file(KFileItem::Unknown, KFileItem::Unknown, url);
  if(file.isReadable())
  {
    if(m_tabEditor->openDocument(url))
    {
      m_actionRecent->addURL(url);
      return;
    }
  }

  m_actionRecent->removeURL(url);
  showSorry(QString("\"") + url.prettyURL() + "\" is unreadable.");
}


void MainWindow::slotFileRecent(const KURL& url)
{
  openFile(url);
}

void MainWindow::slotCloseFile()
{
  m_tabEditor->closeCurrentDocument();
}

void MainWindow::slotSaveCurrentFile() 
{
  saveCurrentFile();
}

void MainWindow::slotSaveCurrentFileAs()
{
  saveCurrentFileAs();
}

bool MainWindow::saveCurrentFile()
{
  if(!m_tabEditor->currentDocumentExistsOnDisk())
  {
    saveCurrentFileAs();
  }
  else if(!m_tabEditor->saveCurrentFile())
  {
    //katepart already show an error message
    //showSorry("Unable to save file");
    return false;
  }
  return true;
}

bool MainWindow::saveCurrentFileAs()
{
  SiteSettings* currentSite = ProtoeditorSettings::self()->currentSiteSettings();
  QString location;
  if(currentSite)
  {
    location = currentSite->localBaseDir().path();
  }
  else
  {
    location = ":protoeditor";
  }

    KEncodingFileDialog::Result res=KEncodingFileDialog::getSaveURLAndEncoding(
      QString(),location,QString::null,this,i18n("Save File"));



  //KURL url = KFileDialog::getSaveURL(location, QString::null, this);
  if(!res.URLs.isEmpty() && checkOverwrite( res.URLs.first() ) )
//  if(!url.isEmpty() && checkOverwrite(url))
  {
    if(m_tabEditor->saveCurrentFileAs(res.URLs.first(), res.encoding))
    {
      return true;
    }
  }
  return false;
}

bool MainWindow::checkOverwrite(KURL u)
{
  if(!u.isLocalFile())
    return true;

  QFileInfo info( u.path() );
  if( !info.exists() )
    return true;

  return KMessageBox::Continue
         == KMessageBox::warningContinueCancel
              ( this,
                i18n( "A file named \"%1\" already exists. Are you sure you want to overwrite it?" ).arg( info.fileName() ),
                i18n( "Overwrite File?" ),
                KGuiItem( i18n( "&Overwrite" ), "filesave", i18n( "Overwrite the file" ) )
              );
}

void MainWindow::slotCloseAllFiles()
{
  m_tabEditor->closeAllDocuments();
}

// void MainWindow::slotSaveFile()
// {
//   if(!m_tabEditor->saveCurrentFile())
//   {
//     showSorry("Unable to save file");
//   }
// }

// void MainWindow::slotSaveFileAs()
// {
//   //Show KFileDialog on the current Site's "local base dir" or
//   //use ::protoeditor to retrieve the last folder used
// 
//   SiteSettings* currentSite = ProtoeditorSettings::self()->currentSiteSettings();
//   QString location;
//   if(currentSite)
//   {
//     location = currentSite->localBaseDir();
//   }
//   else
//   {
//     location = ":protoeditor";
//   }
// 
//   KURL url = KFileDialog::getSaveURL(location, QString::null, this);
// 
//   if(!url.isEmpty())
//   {
//     if(!m_tabEditor->saveCurrentFileAs(url))
//     {
//       showSorry("Unable to save file");
//     }
//   }
// }

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
  if(m_tabEditor->closeAllDocuments())
  {
    close();
  }
  kapp->config()->sync();
}

void MainWindow::closeEvent(QCloseEvent * e)
{
  if(m_tabEditor->closeAllDocuments())
  {
    KMainWindow::closeEvent(e);
  }
}

void MainWindow::showError(const QString& msg) const
{
  KMessageBox::error(0, msg);
}

void MainWindow::slotEditKeys()
{
  KKeyDialog dlg;
  dlg.insert(actionCollection());

  if(m_tabEditor->count() != 0)
  {
    KTextEditor::View* view  = m_tabEditor->currentView();
    if(view)
    {
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
  if (dlg.exec())
  {
    //setupGUI();
    applyMainWindowSettings( KGlobal::config(), autoSaveGroup() );
  }
}

void MainWindow::setEditorStatusMsg(const QString& msg)
{
  m_lbEditorMsg->setText(msg);
}
void MainWindow::setDebugStatusMsg(const QString& msg)
{
  m_lbDebugMsg->setText(msg);
}

void MainWindow::setDebugStatusName(const QString& name)
{
  m_lbDebugName->setText(name);
}

void MainWindow::setLedState(int state)
{
  switch(state)
  {
    case LedOn:
      m_lbLed->setPixmap(QPixmap(UserIcon("indicator_on")));
      break;
/*    case LedWait:
      m_lbLed->setPixmap(QPixmap(UserIcon("indicator_wait")));
      break;*/
    case LedOff:
    default:
      m_lbLed->setPixmap(QPixmap(UserIcon("indicator_off")));
  }
}

void MainWindow::slotShowSettings()
{
  ConfigDlg::showDialog();
}

void MainWindow::slotScriptRun()
{
  QString arg = m_cbArguments->currentText();
  if(!arg.isEmpty()) 
  {
    m_cbArguments->addToHistory(arg);    
  }  

  m_debugger_manager->slotScriptRun();
}

void MainWindow::slotDebugStart()
{
  QString arg = m_cbArguments->currentText();
  if(!arg.isEmpty()) 
  {
    m_cbArguments->addToHistory(arg);    
  }
  m_debugger_manager->slotDebugStart();
}

void MainWindow::slotFocusArgumentBar()
{
  m_cbArguments->setFocus();
  m_cbArguments->lineEdit()->selectAll();
}

// void MainWindow::slotCurrentSiteChanged(const QString&)
// {
//   //this slot has to be connected after the settings connection
//   //so the settings has the current site
//   //disable Profile if not DBG
// 
//   SiteSettings* site = ProtoeditorSettings::self()->currentSiteSettings();
//   if(site)
//   {
//     if(site->debuggerClient() == "DBG")
//     {
//       actionCollection()->action("script_profile")->setEnabled(true);
//     }
//     else
//     {
//       actionCollection()->action("script_profile")->setEnabled(false);
//     }
//   }
//   else
//   {
//     if(ProtoeditorSettings::self()->phpSettings()->defaultDebugger() == "DBG")
//     {
//       actionCollection()->action("script_profile")->setEnabled(true);
//     }
//     else
//     {
//       actionCollection()->action("script_profile")->setEnabled(false);
//     }
//   }
// }

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

MessageListView* MainWindow::messageListView()
{
  return m_messageListView;
}

KTextEdit* MainWindow::edOutput()
{
  return m_edOutput;
}

KHistoryCombo* MainWindow::cbArguments()
{
  return m_cbArguments;
}

#include "mainwindow.moc"
