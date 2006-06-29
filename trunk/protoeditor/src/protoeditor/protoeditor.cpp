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
#include "statusbarwidget.h"

#include "session.h"
#include "protoeditorsettings.h"
#include "sitesettings.h"

#include "phpsettings.h"
#include "perlsettings.h"

#include "executioncontroller.h"

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
QString Protoeditor::fileFilter = "*.php *.pl| Scripts\n"
                                  "*.php| PHP Scripts\n"
                                  "*.pl| Perl Scripts\n"
                                  "*| All Files";



Protoeditor::Protoeditor()
{
  connect(ProtoeditorSettings::self(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  m_executionController = new ExecutionController();
}


Protoeditor::~Protoeditor()
{
  //saves the recent opened files
  m_window->saveRecentEntries();  
  kapp->config()->sync();

  //removes the current text (wich might not have been used in debug/execution)
  //and saves the arguments
  m_window->argumentCombo()->clearEdit();
  ProtoeditorSettings::self()->setArgumentsHistory(
    m_window->argumentCombo()->historyItems());

  //write all settings
  ProtoeditorSettings::self()->writeConfig(true);

  //start getting rid of things
  
  delete m_window;

  Session::dispose();
  ProtoeditorSettings::dispose();
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
  m_window = new MainWindow();

  connect(m_window->editorUI(), SIGNAL(sigEmpty()),
    this, SLOT(slotNoDocumentOpened()));

  connect(m_window->editorUI(), SIGNAL(sigFirstPage()),
    this, SLOT(slotFirstDocumentOpened()));

  //Load all languages
  registerLanguages();
  loadLanguages();

  //Load all language debuggers
  DebuggerFactory::self()->init();  

  //setup the argument toolbar
  m_window->argumentCombo()->insertStringList(ProtoeditorSettings::self()->argumentsHistory());
  m_window->argumentCombo()->clearEdit();
}

MainWindow* Protoeditor::mainWindow()
{
  return m_window;
}

void Protoeditor::openFile()
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


/***************************************************************/

void Protoeditor::slotNoDocumentOpened()
{
    m_window->actionStateChanged("has_nofileopened");
    m_window->setCaption(QString::null);
    m_window->statusBar()->setEditorStatus(QString::null);
}

void Protoeditor::slotFirstDocumentOpened()
{
  m_window->actionStateChanged("has_fileopened");
}

void Protoeditor::registerLanguages()
{
  ProtoeditorSettings::self()->registerLanguage(new PHPSettings());
  ProtoeditorSettings::self()->registerLanguage(new PerlSettings());
}

void Protoeditor::loadSites()
{
  QString currentSiteName = m_window->currentSiteName();

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
    
  m_window->setSiteNames(strsites);
  m_window->setCurrentSite(curr);
    
  ProtoeditorSettings::self()->slotCurrentSiteChanged(m_window->currentSiteName());  
}

void Protoeditor::loadLanguages()
{
  m_window->clearLanguages();

  QValueList<LanguageSettings*> langs = 
    ProtoeditorSettings::self()->languageSettingsList();

  QValueList<LanguageSettings*>::iterator it;
  for(it = langs.begin(); it != langs.end(); ++it)
  {
    if((*it)->isEnabled())
    {
      m_window->addLanguage((*it)->languageName());
    }
  }
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

/****************** File menu slots *****************************/

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

/************************* Script menu slots *********************************/

void Protoeditor::slotAcExecuteScript(const QString& langName)
{  
  m_executionController->executeScript(langName, m_window->argumentCombo()->currentText());
  m_window->statusBar()->setDebugStatusMsg(langName + i18n(" executed"));
}

void Protoeditor::slotAcDebugStart(const QString&)
{  
//   m_executionController->debugStart(langName);
}

void Protoeditor::slotAcDebugStop()
{
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

void Protoeditor::slotProfile()
{
//   KURL url = m_window->editorUI()->currentDocumentURL();
//   m_executionController->profile(langName, url);
}

void Protoeditor::slotAcDebugToggleBp()
{
}

void Protoeditor::slotSettingsChanged()
{
  loadSites();
  loadLanguages();
}

void Protoeditor::showError(const QString& msg) const
{
  m_window->showError(msg);
}

void Protoeditor::showSorry(const QString& msg) const
{
  m_window->showSorry(msg);
}


#include "protoeditor.moc"
