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

DBGConfiguration::DBGConfiguration()
{
}

DBGConfiguration::~DBGConfiguration()
{
}

void DBGConfiguration::setLocalBaseDir(QString localBaseDir) {
  if(localBaseDir.at(localBaseDir.length()-1) == '/') {
    localBaseDir.remove(localBaseDir.length()-1, 1);
  }
  m_localBaseDir = localBaseDir;

}

void DBGConfiguration::setServerBaseDir(QString serverBaseDir) {
  if(serverBaseDir.at(serverBaseDir.length()-1) == '/') {
    serverBaseDir.remove(serverBaseDir.length()-1, 1);
  }
  m_serverBaseDir = serverBaseDir;
}

void DBGConfiguration::setListenPort(int port) {
  m_port = port;
}

void DBGConfiguration::setHost(QString host) {
  if(host.at(host.length()-1) == '/') {
    host.remove(host.length()-1, 1);
  }
  m_host = host;
}

QString DBGConfiguration::localBaseDir() {
  return m_localBaseDir;
}

QString DBGConfiguration::serverBaseDir() {
  return m_serverBaseDir;
}

int DBGConfiguration::listenPort() {
  return m_port;
}

QString DBGConfiguration::host() {
  return m_host;
}

