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

#include "debuggerconfigurations.h"
#include "dbgconfiguration.h"
#include <kconfig.h>

DebuggerConfigurations::DebuggerConfigurations()
  : m_debugger_id(-1)
{
  m_dbgConfiguration = new DBGConfiguration();
  //m_gubedConfiguration = new GubedConfiguration();
}

DebuggerConfigurations::~DebuggerConfigurations()
{
}

void DebuggerConfigurations::setDebuggerId(int id) {
  m_debugger_id = id;
}

int DebuggerConfigurations::debuggerId() {
  return m_debugger_id;
}

DBGConfiguration* DebuggerConfigurations::dbgConfiguration() {
  return m_dbgConfiguration;
}

void DebuggerConfigurations::saveConfigurations(KConfig* config) {
  if(!m_dbgConfiguration) return;

  config->setGroup("Debugger");
  config->writeEntry("Debugger ID", m_debugger_id);
  saveDBGConfiguration(config);
}

void DebuggerConfigurations::readConfigurations(KConfig* config) {
  if(!config->hasGroup ("Debugger")) return;

  config->setGroup("Debugger");
  m_debugger_id = config->readNumEntry("Debugger ID");

  if(!m_dbgConfiguration) return;

  readDBGConfiguration(config);
}

void DebuggerConfigurations::saveDBGConfiguration(KConfig* config) {
  config->setGroup("DBG");
  config->writeEntry("Local base dir", m_dbgConfiguration->localBaseDir());
  config->writeEntry("Server base dir", m_dbgConfiguration->serverBaseDir());
  config->writeEntry("Listen port", m_dbgConfiguration->listenPort());
  config->writeEntry("Host", m_dbgConfiguration->host());
  config->sync();
}

void DebuggerConfigurations::readDBGConfiguration(KConfig* config) {
  if(!config->hasGroup ("DBG")) return;

  config->setGroup("DBG");

  m_dbgConfiguration->setLocalBaseDir(
    config->readEntry("Local base dir"));

  m_dbgConfiguration->setServerBaseDir(
    config->readEntry("Server base dir"));

  m_dbgConfiguration->setListenPort(
    config->readNumEntry("Listen port"));

  m_dbgConfiguration->setHost(
    config->readEntry("Host"));
}


