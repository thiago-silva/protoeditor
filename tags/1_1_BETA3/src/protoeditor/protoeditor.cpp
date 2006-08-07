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
#include "uinterface.h"
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

#include <kaction.h>
#include <kcombobox.h>
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>


Protoeditor* Protoeditor::m_self = 0;


Protoeditor::Protoeditor()
  : m_uinterface(0), m_settings(0), m_session(0),
    m_executionController(0), m_dataController(0)
{
  
}

Protoeditor::~Protoeditor()
{
  //saves the recent opened files
  m_uinterface->saveRecentEntries();
  kapp->config()->sync();

  //removes the current text (wich might not have been used in debug/execution)
  //and saves the arguments
  m_uinterface->argumentCombo()->clearEdit();
  settings()->setArgumentsHistory(
    m_uinterface->argumentCombo()->historyItems());

  //write all settings
  m_settings->writeConfig(true);

  //start getting rid of things
  
  delete m_executionController;

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
  Protoeditor::m_self = 0;
}

void Protoeditor::init(UInterface* ui, const QString& config)
{
    m_fileFilter = i18n("*.php *.pl *py| Scripts\n"
                                  "*.php| PHP Scripts\n"
                                  "*.pl| Perl Scripts\n"
                                  "*.py| Python Scripts\n"
                                  "*| All Files");


  m_settings = new ProtoeditorSettings(config);
  m_session = new Session();

  m_dataController = new DataController();  

  m_executionController = new ExecutionController();
  
  registerLanguages();

  m_uinterface = ui;
  m_uinterface->setup();


  m_executionController->init();

  connect(Protoeditor::self()->session(), SIGNAL(sigError(const QString&)),
      this, SLOT(slotError(const QString&)));

  //settings
  connect(m_settings, SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  //datacontroller
  connect(m_executionController, SIGNAL(sigDebugStarted()),
          m_dataController, SLOT(slotDebugStarted()));

  connect(m_executionController, SIGNAL(sigDebugEnded()),
          m_dataController, SLOT(slotDebugEnded()));

  //execution controller
  connect(m_executionController, SIGNAL(sigDebugStarted()),
          this, SLOT(slotDebugStarted()));

  connect(m_executionController, SIGNAL(sigDebugEnded()),
          this, SLOT(slotDebugEnded()));

  //Load all languages  
  loadLanguages();
  loadSites();

  //setup the argument toolbar
  m_uinterface->argumentCombo()->insertStringList(m_settings->argumentsHistory());
  m_uinterface->argumentCombo()->clearEdit();    
}

UInterface* Protoeditor::uinterface()
{
  return m_uinterface;
}

EditorInterface* Protoeditor::editorUI()
{
  return m_uinterface->editorUI();
}

DebuggerInterface* Protoeditor::debuggerUI()
{
  return m_uinterface->debuggerUI();
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

bool Protoeditor::useCurrentScript()
{
  return m_uinterface->isCurrentScriptActionChecked();
}

void Protoeditor::showError(const QString& msg) const
{
  KMessageBox::error(0, msg);
}


void Protoeditor::showSorry(const QString& msg) const
{
  KMessageBox::sorry(0, msg);
}

/*********** File Menu Slots *********************/


/*********** Script Menu Slots *********************/

void Protoeditor::slotAcExecuteScript(const QString& langName)
{
  m_executionController->executeScript(langName, m_uinterface->argumentCombo()->currentText());  
}

void Protoeditor::slotAcDebugStart(const QString& langName)
{
  m_executionController->debugStart(langName
                                  , m_uinterface->argumentCombo()->currentText()
                                  , m_settings->currentSiteSettings()?false:true);
}

void Protoeditor::slotAcDebugStart()
{
  m_executionController->debugStart(m_uinterface->argumentCombo()->currentText()
                                  , m_settings->currentSiteSettings()?false:true);
}

void Protoeditor::slotAcProfileScript(const QString& langName)
{
  m_executionController->profileScript(langName
                                  , m_uinterface->argumentCombo()->currentText()
                                  , m_settings->currentSiteSettings()?false:true);
}

void Protoeditor::slotAcProfileScript()
{
  m_executionController->profileScript(m_uinterface->argumentCombo()->currentText()
                                  , m_settings->currentSiteSettings()?false:true);
}

void Protoeditor::slotAcDebugStop()
{
  m_uinterface->setDebugMsg(i18n("Stopping..."));
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
  KURL url = m_uinterface->editorUI()->currentDocumentURL();
  int line = m_uinterface->editorUI()->currentDocumentLine();

  if(!m_uinterface->editorUI()->hasBreakpointAt(url, line))
  {
    m_uinterface->editorUI()->markActiveBreakpoint(url, line);
    m_uinterface->debuggerUI()->toggleBreakpoint(url, line);
  }
  else
  {
    m_uinterface->editorUI()->unmarkActiveBreakpoint(url, line);
    m_uinterface->debuggerUI()->toggleBreakpoint(url, line);
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
  m_uinterface->actionStateChanged("has_fileopened");
}

void Protoeditor::slotNoDocumentOpened()
{
  m_uinterface->actionStateChanged("has_nofileopened");
  m_uinterface->changeCaption(QString::null);
}

void Protoeditor::slotGotoLineAtFile(const KURL& url, int line)
{
  m_uinterface->editorUI()->gotoLineAtFile(url, line-1);
}

void Protoeditor::slotError(const QString& message)
{
  showError(message);
}

void Protoeditor::registerLanguages()
{
  m_settings->registerLanguage(new PHPSettings(m_settings->configFile()));
  m_settings->registerLanguage(new PerlSettings(m_settings->configFile()));  
  m_settings->registerLanguage(new PythonSettings(m_settings->configFile()));  
}

void Protoeditor::loadLanguages()
{
  m_uinterface->clearLanguages();

  QValueList<LanguageSettings*> langs = 
    m_settings->languageSettingsList();

  QValueList<LanguageSettings*>::iterator it;
  for(it = langs.begin(); it != langs.end(); ++it)
  {
    if((*it)->isEnabled())
    {
      m_uinterface->addLanguage((*it)->languageName());
    }
  }
  m_uinterface->setCurrentLanguage(m_settings->currentLanguage());
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
    
  m_uinterface->setSiteNames(strsites);
  m_uinterface->setCurrentSite(curr);
    
  m_settings->setCurrentSiteName(m_uinterface->currentSiteName());  
}

void Protoeditor::slotDebugStarted()
{
  m_uinterface->setDebugMsg(i18n("Debug started"));
  m_uinterface->setDebuggerName(m_executionController->currentDebuggerName()); //show the name of the debugger on the StatusBar
  m_uinterface->setLedState(StatusBarWidget::LedOn);          //green led

  //setup the actions stuff
  m_uinterface->actionStateChanged("debug_started");
  m_uinterface->debuggerUI()->prepareForSession();
}

void Protoeditor::slotDebugEnded()
{
  //setup the actions stuff  
  m_uinterface->actionStateChanged("debug_stopped");
  
  m_uinterface->setDebugMsg(i18n("Stopped"));
  m_uinterface->setLedState(StatusBarWidget::LedOff); //red led

  m_uinterface->setDebuggerName("");
  m_uinterface->debuggerUI()->cleanSession();
}

QString Protoeditor::fileFilter()
{
  return m_fileFilter;
}

#include "protoeditor.moc"
