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

#include "protoeditor.h"
#include "mainwindow.h"
#include "editorui.h"
#include "debuggerui.h"
#include "statusbarwidget.h"

#include "session.h"
#include "protoeditorsettings.h"
#include "sitesettings.h"

#include "phpsettings.h"
#include "perlsettings.h"
#include "pythonsettings.h"

#include "datacontroller.h"
#include "executioncontroller.h"

#include "debuggerfactory.h"
#include "configdlg.h"

#include <kaction.h>
#include <kcombobox.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kencodingfiledialog.h>
#include <kmessagebox.h>


Protoeditor* Protoeditor::m_self = 0;

//TODO: i18n this
QString Protoeditor::fileFilter = "*.php *.pl *py| Scripts\n"
                                  "*.php| PHP Scripts\n"
                                  "*.pl| Perl Scripts\n"
                                  "*.py| Python Scripts\n"
                                  "*| All Files";



Protoeditor::Protoeditor()
{
  
}

Protoeditor::~Protoeditor()
{
  //saves the recent opened files
  m_window->saveRecentEntries();  
  kapp->config()->sync();

  //removes the current text (wich might not have been used in debug/execution)
  //and saves the arguments
  m_window->argumentCombo()->clearEdit();
  settings()->setArgumentsHistory(
    m_window->argumentCombo()->historyItems());

  //write all settings
  m_settings->writeConfig(true);

  //start getting rid of things
  
  delete m_executionController;

  delete m_window;
  delete m_configDlg;
  delete m_settings;
  delete m_session;
  
  delete m_dataController;
}

Protoeditor* Protoeditor::self()
{
  if(!Protoeditor::m_self) {
    Protoeditor::m_self = new Protoeditor();
  } 
  return Protoeditor::m_self;
}

void Protoeditor::dispose()
{
  delete Protoeditor::m_self;
}

void Protoeditor::init()
{
  m_settings = new ProtoeditorSettings();
  m_session = new Session();  
  

  m_dataController = new DataController();  

  m_executionController = new ExecutionController();
  
  registerLanguages();

  m_executionController->init();

  m_configDlg = new ConfigDlg();

  m_window = new MainWindow();

  connect(Protoeditor::self()->session(), SIGNAL(sigError(const QString&)),
      this, SLOT(slotError(const QString&)));

  //mainwindow
  connect(m_window, SIGNAL(sigExecuteScript(const QString&)), this,
      SLOT(slotAcExecuteScript(const QString&)));

  connect(m_window, SIGNAL(sigDebugScript(const QString&)), this,
      SLOT(slotAcDebugStart(const QString&)));

  connect(m_window, SIGNAL(sigDebugScript()), this,
      SLOT(slotAcDebugStart()));

  connect(m_window, SIGNAL(sigProfileScript(const QString&)), this,
      SLOT(slotAcProfileScript(const QString&)));

  //settings
  connect(m_settings, SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  //editor ui
  connect(m_window->editorUI(), SIGNAL(sigFirstPage()),
          this, SLOT(slotFirstDocumentOpened()));

  connect(m_window->editorUI(), SIGNAL(sigEmpty()),
          this, SLOT(slotNoDocumentOpened()));

  connect(m_window->debuggerUI(), SIGNAL(sigGotoFileAndLine( const KURL&, int )),
          this, SLOT(slotGotoLineAtFile(const KURL&, int)));
 
  //datacontroller
  connect(m_executionController, SIGNAL(sigDebugStarted()),
          m_dataController, SLOT(slotDebugStarted()));

  connect(m_executionController, SIGNAL(sigDebugEnded()),
          m_dataController, SLOT(slotDebugEnded()));

  connect(m_window->editorUI(), SIGNAL(sigNewPage()),
          m_dataController, SLOT(slotNewDocument()));


  connect(m_window->debuggerUI(), SIGNAL(sigGlobalVarModified(Variable*)),
          m_dataController, SLOT(slotGlobalVarModified(Variable*)));

  connect(m_window->debuggerUI(), SIGNAL(sigLocalVarModified(Variable*)),
          m_dataController, SLOT(slotLocalVarModified(Variable*)));

  connect(m_window->debuggerUI(), SIGNAL(sigWatchAdded(const QString&)),
          m_dataController, SLOT(slotWatchAdded(const QString&)));

  connect(m_window->editorUI(), SIGNAL(sigAddWatch(const QString&)),
          m_dataController, SLOT(slotWatchAdded(const QString&)));
          
  connect(m_window->debuggerUI(), SIGNAL(sigWatchRemoved(Variable*)),
          m_dataController, SLOT(slotWatchRemoved(Variable*)));

  connect(m_window->debuggerUI(), SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)),
          m_dataController, SLOT(slotBreakpointCreated(DebuggerBreakpoint*)));

  connect(m_window->debuggerUI(), SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
          m_dataController, SLOT(slotBreakpointChanged(DebuggerBreakpoint*)));

  connect(m_window->debuggerUI(), SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)),
          m_dataController, SLOT(slotBreakpointRemoved(DebuggerBreakpoint*)));

  connect(m_window->debuggerUI(),SIGNAL(sigStackchanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)), 
          m_dataController, SLOT(slotStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)));  

  connect(m_window->debuggerUI(), SIGNAL(sigStackchanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)),
          m_dataController, SLOT(slotStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)));

  connect(m_window->debuggerUI(), SIGNAL(sigWatchModified(Variable*)),
          m_dataController, SLOT(slotLocalVarModified(Variable*)));

  //execution controller
  connect(m_window->debuggerUI(), SIGNAL(sigExecuteCmd(const QString&)),
          m_executionController, SLOT(slotExecuteCmd(const QString&)));

  connect(m_executionController, SIGNAL(sigDebugStarted()),
          this, SLOT(slotDebugStarted()));

  connect(m_executionController, SIGNAL(sigDebugEnded()),
          this, SLOT(slotDebugEnded()));

  connect(m_window->debuggerUI(), SIGNAL(sigNeedChildren(int, Variable*)),
          m_executionController, SLOT(slotNeedChildren(int, Variable*)));


  //Load all languages  
  loadLanguages();
  loadSites();

  //Load all language debuggers

  //setup the argument toolbar
  m_window->argumentCombo()->insertStringList(m_settings->argumentsHistory());
  m_window->argumentCombo()->clearEdit();    
}

MainWindow* Protoeditor::mainWindow()
{
  return m_window;
}

EditorUI* Protoeditor::editorUI()
{
  return m_window->editorUI();
}

DebuggerUI* Protoeditor::debuggerUI()
{
  return m_window->debuggerUI();
}

ProtoeditorSettings* Protoeditor::settings()
{
  return m_settings;
}

Session* Protoeditor::session()
{
  return m_session;
}

ExecutionController* Protoeditor::executionController()
{
  return m_executionController;
}

DataController*  Protoeditor::dataController()
{
  return m_dataController;
}

ConfigDlg* Protoeditor::configDlg()
{
  return m_configDlg;
}

void Protoeditor::openFile()
{
  //Show KFileDialog on the current Site's "local base dir" or
  //use ::protoeditor to retrieve the last folder used

  SiteSettings* currentSite = m_settings->currentSiteSettings();
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
      location, Protoeditor::fileFilter, m_window);

  if(strList.count())
  {
    for(QStringList::Iterator it = strList.begin(); it != strList.end(); ++it )
    {
      openFile(KURL::fromPathOrURL(*it));
    }
  }
}

void Protoeditor::openFile(const KURL& url)
{
  KFileItem file(KFileItem::Unknown, KFileItem::Unknown, url);
  if(file.isReadable())
  {
    if(m_window->editorUI()->openDocument(url))
    {
      m_window->addRecentURL(url);
      return;
    }
  }

  m_window->removeRecentURL(url);
  m_window->showSorry(QString("\"") + url.prettyURL() + i18n("\" is unreadable."));
}

bool Protoeditor::saveCurrentFile()
{
  if(!m_window->editorUI()->currentDocumentExistsOnDisk())
  {
    saveCurrentFileAs();
  }
  else if(!m_window->editorUI()->saveCurrentFile())
  {
    //katepart already show an error message
    //don't show any message here
    return false;
  }
  return true;
}

bool Protoeditor::saveCurrentFileAs()
{
  SiteSettings* currentSite = m_settings->currentSiteSettings();
  QString location;
  if(currentSite)
  {
    location = currentSite->localBaseDir().path();
  }
  else
  {
    location = ":protoeditor";
  }

    KEncodingFileDialog::Result res = 
      KEncodingFileDialog::getSaveURLAndEncoding(
        QString(), location, QString::null, m_window, i18n("Save File"));


  if(!res.URLs.isEmpty() && checkOverwrite(res.URLs.first()))
  {
    if(m_window->editorUI()->saveCurrentFileAs(res.URLs.first(), res.encoding))
    {
      return true;
    }
  }
  return false;
}

bool Protoeditor::useCurrentScript()
{
  return m_window->isCurrentScriptActionChecked();
}

void Protoeditor::showError(const QString& msg) const
{
  m_window->showError(msg);
}


void Protoeditor::showSorry(const QString& msg) const
{
  m_window->showSorry(msg);
}

/*********** File Menu Slots *********************/

void Protoeditor::slotAcNewFile()
{
  m_window->editorUI()->createNew();
}

void Protoeditor::slotAcOpenFile()
{
  openFile();
}

void Protoeditor::slotAcFileRecent(const KURL& url)
{
  openFile(url);
}

void Protoeditor::slotAcSaveCurrentFile() 
{
  saveCurrentFile();
}

void Protoeditor::slotAcSaveCurrentFileAs()
{
  saveCurrentFileAs();
}

void Protoeditor::slotAcCloseFile()
{
  m_window->editorUI()->closeCurrentDocument();
}

void Protoeditor::slotAcCloseAllFiles()
{
  m_window->editorUI()->closeAllDocuments();
}

void Protoeditor::slotAcQuit()
{
  if(m_window->editorUI()->closeAllDocuments())
  {
    m_window->close();
  }
}

/*********** Script Menu Slots *********************/

void Protoeditor::slotAcExecuteScript(const QString& langName)
{
  m_executionController->executeScript(langName, m_window->argumentCombo()->currentText());  
}

void Protoeditor::slotAcDebugStart(const QString& langName)
{
  m_executionController->debugStart(langName
                                  , m_window->argumentCombo()->currentText()
                                  , m_settings->currentSiteSettings()?false:true);
}

void Protoeditor::slotAcDebugStart()
{
  m_executionController->debugStart(m_window->argumentCombo()->currentText()
                                  , m_settings->currentSiteSettings()?false:true);
}

void Protoeditor::slotAcProfileScript(const QString& langName)
{
  m_executionController->profileScript(langName
                                  , m_window->argumentCombo()->currentText()
                                  , m_settings->currentSiteSettings()?false:true);
}

void Protoeditor::slotAcProfileScript()
{
  m_executionController->profileScript(m_window->argumentCombo()->currentText()
                                  , m_settings->currentSiteSettings()?false:true);
}

void Protoeditor::slotAcDebugStop()
{
  m_window->statusBar()->setDebugMsg(i18n("Stopping..."));
  m_executionController->debugStop();
}

void Protoeditor::slotAcDebugRunToCursor()
{
  m_executionController->debugRunToCursor();
}

void Protoeditor::slotAcDebugStepOver()
{
  m_executionController->debugStepOver();
}

void Protoeditor::slotAcDebugStepInto()
{
  m_executionController->debugStepInto();
}

void Protoeditor::slotAcDebugStepOut()
{
  m_executionController->debugStepOut();
}

void Protoeditor::slotAcDebugToggleBp()
{
  KURL url = m_window->editorUI()->currentDocumentURL();
  int line = m_window->editorUI()->currentDocumentLine();

  if(!m_window->editorUI()->hasBreakpointAt(url, line))
  {
    m_window->editorUI()->markActiveBreakpoint(url, line);
  }
  else
  {
    m_window->editorUI()->unmarkActiveBreakpoint(url, line);
  }
}

/*********** Global settings changed *********************/

void Protoeditor::slotSettingsChanged()
{
  loadSites();
  loadLanguages();
}

/*********** UI slots *********************/

void Protoeditor::slotFirstDocumentOpened()
{
  m_window->actionStateChanged("has_fileopened");
}

void Protoeditor::slotNoDocumentOpened()
{
  m_window->actionStateChanged("has_nofileopened");
  m_window->setCaption(QString::null);
  m_window->statusBar()->setEditorStatus(QString::null);
}

void Protoeditor::slotGotoLineAtFile(const KURL& url, int line)
{
  m_window->editorUI()->gotoLineAtFile(url, line-1);
}

void Protoeditor::slotError(const QString& message)
{
  showError(message);
}

void Protoeditor::registerLanguages()
{
  m_settings->registerLanguage(new PHPSettings());
  m_settings->registerLanguage(new PerlSettings());  
  m_settings->registerLanguage(new PythonSettings());  
}

void Protoeditor::loadLanguages()
{
  m_window->clearLanguages();

  QValueList<LanguageSettings*> langs = 
    m_settings->languageSettingsList();

  QValueList<LanguageSettings*>::iterator it;
  for(it = langs.begin(); it != langs.end(); ++it)
  {
    if((*it)->isEnabled())
    {
      m_window->addLanguage((*it)->languageName());
    }
  }
  m_window->setCurrentLanguage(m_settings->currentLanguage());
}

void Protoeditor::loadSites()
{
  QString currentSiteName = m_settings->currentSiteName();

  QStringList strsites;

  strsites << ProtoeditorSettings::LocalSiteName;

  QValueList<SiteSettings*> sitesList = m_settings->siteSettingsList();
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
    
  m_window->setSiteNames(strsites);
  m_window->setCurrentSite(curr);
    
  m_settings->setCurrentSiteName(m_window->currentSiteName());  
}

bool Protoeditor::checkOverwrite(const KURL& u)
{
  if(!u.isLocalFile())
    return true;

  QFileInfo info( u.path() );
  if( !info.exists() )
    return true;

  return KMessageBox::Continue
         == KMessageBox::warningContinueCancel
              ( m_window,
                i18n("A file named \"%1\" already exists. Are you sure you want to overwrite it?").arg(info.fileName()),
                i18n("Overwrite File?"),
                KGuiItem(i18n("&Overwrite"), "filesave", i18n("Overwrite the file"))
              );
}

void Protoeditor::slotDebugStarted()
{
  m_window->statusBar()->setDebugMsg(i18n("Debug started"));
  m_window->statusBar()->setDebuggerName(m_executionController->currentDebuggerName()); //show the name of the debugger on the StatusBar
  m_window->statusBar()->setLedState(StatusBarWidget::LedOn);          //green led

  //setup the actions stuff
  m_window->actionStateChanged("debug_started");
  m_window->actionCollection()->action("debug_start")->setText(i18n("Continue"));

  m_window->actionCollection()->action("site_selection")->setEnabled(false);
  m_window->debuggerUI()->prepareForSession();
}

void Protoeditor::slotDebugEnded()
{
  //setup the actions stuff
  m_window->actionCollection()->action("site_selection")->setEnabled(true);
  m_window->actionStateChanged("debug_stopped");
  m_window->actionCollection()->action("debug_start")->setText(i18n("Start Debug"));
  
  m_window->statusBar()->setDebugMsg(i18n("Stopped"));
  m_window->statusBar()->setLedState(StatusBarWidget::LedOff); //red led

  m_window->statusBar()->setDebuggerName("");
  m_window->debuggerUI()->cleanSession();
}

#include "protoeditor.moc"
