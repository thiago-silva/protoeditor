/***************************************************************************
 *   Copyright (C) 2004 by Jesterman                                       *
 *   jesterman@brturbo.com                                                 *
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
#include "dbgfileinfo.h"
#include "dbgconfiguration.h"

DBGFileInfo::DBGFileInfo(DBGConfiguration* conf)
    : m_dbgConfiguration(conf), m_statusUpdated(false)
{}


DBGFileInfo::~DBGFileInfo()
{}

QString DBGFileInfo::toURI(const QString& filePath)
{
  QString uri = filePath;
  return uri.remove(0, m_dbgConfiguration->localBaseDir().length());
}

QString DBGFileInfo::toRemoteFilePath(const QString& localFilePath)
{
  QString localFile  = localFilePath;
  QString serverPath = m_dbgConfiguration->serverBaseDir();
  QString localPath  = m_dbgConfiguration->localBaseDir();
  return serverPath + localFile.remove(0, localPath.length());
}

QString DBGFileInfo::toLocalFilePath(const QString& remoteFilePath)
{
  QString remoteFile  = remoteFilePath;
  QString serverPath  = m_dbgConfiguration->serverBaseDir();
  QString localPath   = m_dbgConfiguration->localBaseDir();
  return localPath + remoteFile.remove(0, serverPath.length());
}

const QString& DBGFileInfo::moduleName(int modno)
{
  return m_fileMap[modno];
}

int DBGFileInfo::moduleNumber(const QString& localFilePath)
{
  QMap<int, QString>::Iterator it;
  for( it = m_fileMap.begin(); it != m_fileMap.end(); ++it ) {
    if(it.data() == localFilePath) {
      return it.key();
    }
  }

  return 0;
}

void DBGFileInfo::setModulePath(int modno, const QString& remotefilePath)
{
  m_fileMap[modno] = toLocalFilePath(remotefilePath);

  m_statusUpdated = true;
}

void DBGFileInfo::clearStatus()
{
  m_statusUpdated = false;
}

bool  DBGFileInfo::updated() {
  return m_statusUpdated;
}

void DBGFileInfo::clear() {
  m_fileMap.clear();
}
