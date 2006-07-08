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
 
#include "protoeditorsettings.h"
#include "sitesettings.h"
#include "extappsettings.h"
#include "debuggersettingsinterface.h"
#include "languagesettings.h"

#include <kglobal.h>
#include <kconfig.h>
#include <kurl.h>

QString ProtoeditorSettings::LocalSiteName = "-- Local --";

ProtoeditorSettings::ProtoeditorSettings()
  :  QObject(), KConfigSkeleton( QString::fromLatin1( "protoeditorrc" ) )
{
  setCurrentGroup(QString::fromLatin1( "Protoeditor" ));

  KConfigSkeleton::ItemString  *itemCurrentSite;
  itemCurrentSite = new KConfigSkeleton::ItemString(currentGroup(), QString::fromLatin1( "CurrentSite" ), m_currentSiteName);
  addItem( itemCurrentSite, QString::fromLatin1( "CurrentSite" ) );

  KConfigSkeleton::ItemStringList *itemArgHistory;
  itemArgHistory = new ItemStringList(currentGroup(), QString::fromLatin1("ArgumentHistory"), m_argumentsHistory);
  addItem( itemArgHistory, QString::fromLatin1( "ArgumentHistory" ) );

  KConfigSkeleton::ItemString  *itemCurrentSiteSite;
  itemCurrentSiteSite = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "CurrentSite" ), m_currentSiteName );
  addItem( itemCurrentSiteSite, QString::fromLatin1( "CurrentSite" ) );

  readConfig();

  m_extApptSettings = new ExtAppSettings();

  //load all Sites
  loadSites();
}

void ProtoeditorSettings::loadSites()
{
  KConfig* config = KGlobal::config();
  SiteSettings* site;
  int i = 0;
  while(config->hasGroup(QString("Site_%1").arg(i))) {
    site = new SiteSettings(QString::number(i));
    m_siteSettingsMap[site->name()] = site;
    i++;
  }
}


ProtoeditorSettings::~ProtoeditorSettings()
{
  QMap<QString, LanguageSettings*>::iterator it;
  LanguageSettings* l;
  for(it = m_langSettingsMap.begin(); it != m_langSettingsMap.end(); ++it) {
     l = it.data();
     delete l;
  }

  delete m_extApptSettings;
}

void ProtoeditorSettings::setArgumentsHistory(const QStringList& args)
{
  m_argumentsHistory.clear();

  //get only the first 20 strings  
  int c = 0;
  for(QStringList::const_iterator it = args.begin(); it != args.end(); it++, c++) 
  {
    if(c >= 20) break;
    m_argumentsHistory << (*it);
  }
}

QStringList ProtoeditorSettings::argumentsHistory()
{
  return m_argumentsHistory;
}

void ProtoeditorSettings::registerLanguage(LanguageSettings* langSettings)
{
  m_langSettingsMap[langSettings->languageName()] = langSettings;
}

LanguageSettings* ProtoeditorSettings::languageSettings(const QString& name)
{
  if(m_langSettingsMap.contains(name))
  {
    return m_langSettingsMap[name];
  }
  else
  {
    return NULL;
  }
}

QValueList<LanguageSettings*> ProtoeditorSettings::languageSettingsList()
{
  return m_langSettingsMap.values();
}

QString ProtoeditorSettings::currentSiteName() {
  return m_currentSiteName;
}


QStringList ProtoeditorSettings::supportedLanguages()
{
  QStringList list;
  QMap<QString, LanguageSettings*>::iterator it;
  for(it = m_langSettingsMap.begin(); it != m_langSettingsMap.end(); ++it) {
     list << it.data()->languageName();
  }
  return list;
}

SiteSettings* ProtoeditorSettings::currentSiteSettings()
{
  return siteSettings(m_currentSiteName);
}

SiteSettings* ProtoeditorSettings::siteSettings(const QString& name)
{
  if(m_siteSettingsMap.contains(name))
  {
    return m_siteSettingsMap[name];
  }
  else
  {
    return NULL;
  }
}

QValueList<SiteSettings*> ProtoeditorSettings::siteSettingsList()
{
  return m_siteSettingsMap.values();
}

ExtAppSettings* ProtoeditorSettings::extAppSettings()
{
  return m_extApptSettings;
}

void ProtoeditorSettings::clearSites()
{
  QMap<QString, SiteSettings*>::iterator sit;
  SiteSettings* s;
  for(sit = m_siteSettingsMap.begin(); sit != m_siteSettingsMap.end(); ++sit) {
     s = sit.data();
     s->remove();     
     delete s;
  }
  m_siteSettingsMap.clear();
}

void ProtoeditorSettings::addSite(int number, const QString& name, const KURL& url,
               const KURL& remoteBaseDir, const KURL& localBaseDir, const KURL& defaultFile,
               const QString& debuggerNme, const QMap<QString,QString>& mappings)

{
  SiteSettings* s = new SiteSettings(QString::number(number));
  s->load(name, url, remoteBaseDir, localBaseDir, defaultFile, debuggerNme, mappings);

  m_siteSettingsMap[s->name()] = s;
}

void ProtoeditorSettings::writeConfig(bool silent)
{
  //parent
  KConfigSkeleton::writeConfig();

  //saves all language settings
  QMap<QString, LanguageSettings*>::iterator it;
  for(it = m_langSettingsMap.begin(); it != m_langSettingsMap.end(); ++it) {
     it.data()->writeConfig();
  }

  //external applications
  m_extApptSettings->writeConfig();

  writeSiteConf();

  if(!silent) {
    emit sigSettingsChanged();
  }
}

void ProtoeditorSettings::writeSiteConf()
{
  QMap<QString, SiteSettings*>::iterator sit;
  for(sit = m_siteSettingsMap.begin(); sit != m_siteSettingsMap.end(); ++sit) {
    sit.data()->writeConfig();
  }
}

void ProtoeditorSettings::slotCurrentSiteChanged(const QString& sitename)
{
  if(sitename == ProtoeditorSettings::LocalSiteName) 
  {
    m_currentSiteName = "";
  }
  else
  {
    m_currentSiteName = sitename;  
  }
}

#include "protoeditorsettings.moc"
