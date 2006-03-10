/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
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
#include "phpsettings.h"
#include "sitesettings.h"
#include "extoutputsettings.h"
#include "debuggersettingsinterface.h"

#include <kglobal.h>
#include <kconfig.h>

ProtoeditorSettings* ProtoeditorSettings::m_self = 0;

ProtoeditorSettings::ProtoeditorSettings()
  :  QObject(), KConfigSkeleton( QString::fromLatin1( "protoeditorrc" ) )
{
  KConfigSkeleton::ItemString  *itemCurrentSite;
  itemCurrentSite = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "CurrentSite" ), m_currentSiteName);
  addItem( itemCurrentSite, QString::fromLatin1( "CurrentSite" ) );

  readConfig();
  //--

  m_phpSettings = new PHPSettings();
  m_extOutputSettings = new ExtOutputSettings();

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
}

ProtoeditorSettings* ProtoeditorSettings::self()
{
  if(!m_self) {
    m_self = new ProtoeditorSettings();
  }

  return m_self;
}

QString ProtoeditorSettings::currentSiteName() {
  return m_currentSiteName;
}

void ProtoeditorSettings::registerDebuggerSettings(DebuggerSettingsInterface* dsettings, const QString& name)
{
  m_debuggerSettingsMap[name] = dsettings;
}

DebuggerSettingsInterface*  ProtoeditorSettings::debuggerSettings(const QString& name)
{
  return m_debuggerSettingsMap[name];
}

QValueList<DebuggerSettingsInterface*> ProtoeditorSettings::debuggerSettingsList()
{
  return m_debuggerSettingsMap.values();

}

SiteSettings* ProtoeditorSettings::currentSiteSettings()
{
  return siteSettings(m_currentSiteName);
}

SiteSettings* ProtoeditorSettings::siteSettings(const QString& name)
{
  return m_siteSettingsMap[name];
}

QValueList<SiteSettings*> ProtoeditorSettings::siteSettingsList()
{
  return m_siteSettingsMap.values();
}
PHPSettings* ProtoeditorSettings::phpSettings()
{
  return m_phpSettings;
}

ExtOutputSettings* ProtoeditorSettings::extOutputSettings()
{
  return m_extOutputSettings;
}

void ProtoeditorSettings::clearSites()
{
  QMap<QString, SiteSettings*>::iterator sit;
  SiteSettings* s;
  for(sit = m_siteSettingsMap.begin(); sit != m_siteSettingsMap.end(); ++sit) {
     s = sit.data();
     s->config()->deleteGroup(s->currentGroup());
     delete s;
  }
  m_siteSettingsMap.clear();
}

void ProtoeditorSettings::addSite(int number, const QString& name, const QString& url,
               const QString& remoteBaseDir, const QString& localBaseDir, const QString& defaultFile,
               const QString& debuggerClient)

{
  SiteSettings* s = new SiteSettings(QString::number(number));
  s->load(name, url,/* port, */remoteBaseDir, localBaseDir, defaultFile,
    /* TODO: */
          false, debuggerClient);

  m_siteSettingsMap[s->name()] = s;
}

void ProtoeditorSettings::writeConfig(bool silent)
{
  KConfigSkeleton::writeConfig();

  m_phpSettings->writeConfig();
  m_extOutputSettings->writeConfig();

  writeDebuggersConf();
  writeSiteConf();

  if(!silent) {
    emit sigSettingsChanged();
  }
}

void ProtoeditorSettings::writeDebuggersConf()
{
  QMap<QString, DebuggerSettingsInterface*>::iterator dit;
  for(dit = m_debuggerSettingsMap.begin(); dit != m_debuggerSettingsMap.end(); ++dit) {
    dit.data()->writeConfig();
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
  m_currentSiteName = sitename;  
}

#include "protoeditorsettings.moc"
