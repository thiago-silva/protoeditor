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
#include "sitesettings.h"
#include "debuggermanager.h"
#include "configdlg.h"
#include "protoeditorsettings.h"
#include "phpsettings.h"

#include "editorui.h"
#include "debuggerui.h"
#include "statusbarwidget.h"

#include <kapplication.h>
#include <kaction.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <kcombobox.h>
#include <qvaluelist.h>

#include <kdialogbase.h>
#include <kencodingfiledialog.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qsplitter.h>

#include <ktexteditor/view.h> 

MainWindow::MainWindow(QWidget* parent, const char* name, WFlags fl)
    : KParts::MainWindow(parent, name, fl),  m_debuggerSettings(0), m_browserSettings(0)
{
  if(!name) 
  { 
    setName("MainWindow"); 
  }

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

  m_siteAction     = new KSelectAction(i18n("Site"), 0, actionCollection(), "site_selection");
  m_siteAction->setComboWidth(150);

  connect(m_siteAction, SIGNAL(activated(const QString&)),
      ProtoeditorSettings::self(), SLOT(slotCurrentSiteChanged(const QString&)));

  m_activeScriptAction = new KToggleAction(i18n("Use Current Script"), "attach", 0, actionCollection(), "use_current_script");

  (void)new KAction(i18n("Run in Console"), "gear", "Shift+F9", this,
                    SLOT(slotScriptRun()), actionCollection(), "script_run");

  (void)new KAction(i18n("Start Debug"), "dbgstart", "F9", this,
                    SLOT(slotDebugStart()), actionCollection(), "debug_start");

  (void)new KAction(i18n("Stop Debug"), "stop", "Escape", m_debugger_manager,
                    SLOT(slotDebugStop()), actionCollection(), "debug_stop");

  (void)new KAction(i18n("Run to Cursor"), "dbgrunto", 0, m_debugger_manager,
                    SLOT(slotDebugRunToCursor()), actionCollection(), "debug_run_to_cursor");


  (void)new KAction(i18n("Step Over"), "dbgnext", "F10", m_debugger_manager,
                    SLOT(slotDebugStepOver()), actionCollection(), "debug_step_over");

  (void)new KAction(i18n("Step Into"), "dbgstep", "F11", m_debugger_manager,
                    SLOT(slotDebugStepInto()), actionCollection(), "debug_step_into");


  (void)new KAction(i18n("Step Out"), "dbgstepout", "F12", m_debugger_manager,
                    SLOT(slotDebugStepOut()), actionCollection(), "debug_step_out");

 (void)new KAction(i18n("Profile (DBG only)"), "math_sum", "Alt+P", 
      m_debugger_manager, SLOT(slotProfile()), actionCollection(), "script_profile");
  
  (void)new KAction(i18n("Toggle Breakpoint"), "activebreakpoint", "Alt+B", m_debugger_manager,
                    SLOT(slotDebugToggleBp()), actionCollection(), "debug_toggle_bp");

  (void)new KAction(i18n("Add Watch"), "math_brace", 0, m_editorUI,
                    SLOT(slotAddWatch()), actionCollection(), "editor_add_watch");


  KWidgetAction* comboAction = new KWidgetAction( m_cbArguments, i18n("Argument Bar "), 0,
                  0, 0, actionCollection(), "argument_combobox" );
  comboAction->setShortcutConfigurable(false);
  comboAction->setAutoSized(true);


  (void)new KAction(i18n("Clear Arguments"), "clear_left", 0, m_cbArguments,
                    SLOT(clearEdit()), actionCollection(), "argument_clear");

  (void)new KWidgetAction(new QLabel(i18n("Arguments: "), this), i18n("Arguments "), "Alt+a", 
      this, SLOT(slotFocusArgumentBar()), actionCollection(), "argument_label" );
  

  setStandardToolBarMenuEnabled(true);
}

void MainWindow::createWidgets()
{
  setCentralWidget(new QWidget(this));
  QVBoxLayout* mainFormBaseLayout = new QVBoxLayout(centralWidget(), 1, 1);

  QSplitter* splitter = new QSplitter(centralWidget());
  splitter->setOrientation( Qt::Vertical  );
  mainFormBaseLayout->addWidget(splitter);

  m_editorUI = new EditorUI(splitter, this);
  splitter->setCollapsible(m_editorUI, false);
  splitter->setOpaqueResize(true);
 
  
  m_debuggerUI = new DebuggerUI(splitter);
  splitter->setCollapsible(m_debuggerUI, false);
  m_debuggerUI->setGeometry(0,0,0,height()/15);

  m_statusBar = new StatusBarWidget(this);

  m_cbArguments = new KHistoryCombo(true, this);
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
  m_editorUI->createNew();
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
      location, i18n("*.php| PHP Scripts\n*|All Files"), this);

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
    if(m_editorUI->openDocument(url))
    {
      m_actionRecent->addURL(url);
      return;
    }
  }

  m_actionRecent->removeURL(url);
  showSorry(QString("\"") + url.prettyURL() + i18n("\" is unreadable."));
}


void MainWindow::slotFileRecent(const KURL& url)
{
  openFile(url);
}

void MainWindow::slotCloseFile()
{
  m_editorUI->closeCurrentDocument();
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
  if(!m_editorUI->currentDocumentExistsOnDisk())
  {
    saveCurrentFileAs();
  }
  else if(!m_editorUI->saveCurrentFile())
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
    if(m_editorUI->saveCurrentFileAs(res.URLs.first(), res.encoding))
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
  m_editorUI->closeAllDocuments();
}

void MainWindow::slotClose()
{
  m_actionRecent->saveEntries(kapp->config());
  kapp->config()->sync();
}

void MainWindow::slotQuit()
{
  if(m_editorUI->closeAllDocuments())
  {
    close();
  }
  kapp->config()->sync();
}

void MainWindow::closeEvent(QCloseEvent * e)
{
  if(m_editorUI->closeAllDocuments())
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

  if(m_editorUI->count() != 0)
  {
    KTextEditor::View* view  = m_editorUI->currentView();
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
    applyMainWindowSettings( KGlobal::config(), autoSaveGroup() );
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

  m_editorUI->saveExistingFiles();
  m_debugger_manager->slotScriptRun();
}

void MainWindow::slotDebugStart()
{
  QString arg = m_cbArguments->currentText();
  if(!arg.isEmpty()) 
  {
    m_cbArguments->addToHistory(arg);    
  }

  m_editorUI->saveExistingFiles();

  m_debugger_manager->slotDebugStart();
}

void MainWindow::slotFocusArgumentBar()
{
  m_cbArguments->setFocus();
  m_cbArguments->lineEdit()->selectAll();
}

void MainWindow::showSorry(const QString& msg) const
{
  KMessageBox::sorry(0, msg);
}

EditorUI* MainWindow::editorUI()
{
  return m_editorUI;
}

DebuggerUI* MainWindow::debuggerUI()
{
  return m_debuggerUI;
}

StatusBarWidget*  MainWindow::statusBar()
{
  return m_statusBar;
}

KHistoryCombo* MainWindow::cbArguments()
{
  return m_cbArguments;
}

#include "mainwindow.moc"
