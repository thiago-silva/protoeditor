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

#include "dbgconfiguration.h"

DBGConfiguration::DBGConfiguration(const QString& localBaseDir, const QString& serverBaseDir,
                                   int listenPort, const QString& serverHost)
  : m_localBaseDir(localBaseDir), m_serverBaseDir(serverBaseDir),
    m_listenPort(listenPort), m_serverHost(serverHost)
{}

DBGConfiguration::~DBGConfiguration()
{}

void DBGConfiguration::setLocalBaseDir(const QString& localBaseDir)
{
  m_localBaseDir = localBaseDir;
  if(m_localBaseDir.at(m_localBaseDir.length()-1) == '/') {
    m_localBaseDir.remove(m_localBaseDir.length()-1, 1);
  }
}

void DBGConfiguration::setServerBaseDir(const QString& serverBaseDir)
{
  m_serverBaseDir = serverBaseDir;
  if(m_serverBaseDir.at(m_serverBaseDir.length()-1) == '/') {
    m_serverBaseDir.remove(m_serverBaseDir.length()-1, 1);
  }
}

void DBGConfiguration::setListenPort(int port)
{
  m_listenPort = port;
}

void DBGConfiguration::setServerHost(const QString& host)
{
  m_serverHost = host;
  if(m_serverHost.at(m_serverHost.length()-1) == '/') {
    m_serverHost.remove(host.length()-1, 1);
  }
}

const QString& DBGConfiguration::localBaseDir() const
{
  return m_localBaseDir;
}

const QString& DBGConfiguration::serverBaseDir() const
{
  return m_serverBaseDir;
}

int DBGConfiguration::listenPort() const
{
  return m_listenPort;
}

const QString& DBGConfiguration::serverHost() const
{
  return m_serverHost;
}

