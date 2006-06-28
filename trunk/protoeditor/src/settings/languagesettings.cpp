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

#include "languagesettings.h"
#include "debuggersettingsinterface.h"

#include <qstring.h>

LanguageSettings::LanguageSettings(const QString& conf)
    : KConfigSkeleton(conf) 
{
}

LanguageSettings::~LanguageSettings() 
{
}
  
void LanguageSettings::writeConfig()
{
  KConfigSkeleton::writeConfig();

  QMap<QString, DebuggerSettingsInterface*>::iterator it;
  for(it = m_debuggerSettingsMap.begin(); it != m_debuggerSettingsMap.end(); ++it) {
    it.data()->writeConfig();
  }
}

void LanguageSettings::registerDebuggerSettings(const QString& name, DebuggerSettingsInterface* dbsettings)
{
  m_debuggerSettingsMap[name] = dbsettings;
}

DebuggerSettingsInterface* LanguageSettings::debuggerSettings(const QString& name)
{
  if(m_debuggerSettingsMap.contains(name))
  {
    return m_debuggerSettingsMap[name];
  }
  else
  {
    return NULL;
  }
}

QValueList<DebuggerSettingsInterface*> LanguageSettings::debuggerSettingsList()
{
  return m_debuggerSettingsMap.values();
}
  

