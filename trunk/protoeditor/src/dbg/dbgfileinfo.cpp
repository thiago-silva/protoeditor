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
#include "sitesettings.h"

DBGFileInfo::DBGFileInfo()
    : m_site(0), m_statusUpdated(false)
{}


DBGFileInfo::~DBGFileInfo()
{}

void DBGFileInfo::setSite(SiteSettings* site)
{
  m_site = site;
}

QString DBGFileInfo::toURI(const QString& filePath)
{
  if(!m_site) return filePath;

  QString uri = filePath;
  return uri.remove(0, m_site->localBaseDir().length());
}

QString DBGFileInfo::toRemoteFilePath(const QString& localFilePath)
{
  if(!m_site) return localFilePath;

  QString localFile  = localFilePath;
  QString serverPath = m_site->remoteBaseDir();
  QString localPath  = m_site->localBaseDir();
  return serverPath + localFile.remove(0, localPath.length());
}

QString DBGFileInfo::toLocalFilePath(const QString& remoteFilePath)
{
  if(!m_site) return remoteFilePath;

  QString remoteFile  = remoteFilePath;
  QString serverPath  = m_site->remoteBaseDir();
  QString localPath   = m_site->localBaseDir();
  return localPath + remoteFile.remove(0, serverPath.length());
}

const QString& DBGFileInfo::moduleName(int modno)
{
  return m_fileMap[modno];
}

int DBGFileInfo::moduleNumber(const QString& localFilePath)
{
  QMap<int, QString>::Iterator it;
  for( it = m_fileMap.begin(); it != m_fileMap.end(); ++it )
  {
    if(it.data() == localFilePath)
    {
      return it.key();
    }
  }

  return 0;
}

void DBGFileInfo::addModuleInfo(int modno, const QString& remotefilePath)
{
  m_fileMap[modno] = toLocalFilePath(remotefilePath);

  m_statusUpdated = true;
}

void DBGFileInfo::addContextInfo(int ctxid, int modno, int lineno)
{
  m_contextList.append(ContextData(ctxid, modno, lineno));
}

void DBGFileInfo::setContextname(int ctxid, const QString& name)
{
  QValueList<ContextData>::iterator it;
  for(it = m_contextList.begin(); it != m_contextList.end(); ++it)
  {
    if((*it).ctxid == ctxid)
    {
      (*it).ctxname = name;
    }
  }
}

int DBGFileInfo::contextId(int modno, int lineno)
{
  QValueList<ContextData>::iterator it;
  for(it = m_contextList.begin(); it != m_contextList.end(); ++it)
  {
    if(((*it).modno == modno) &&
        ((*it).lineno == lineno))

    {
      return (*it).ctxid;
    }
  }
  return -1;
}

QString DBGFileInfo::contextName(int ctxid)
{
  QValueList<ContextData>::iterator it;
  for(it = m_contextList.begin(); it != m_contextList.end(); ++it)
  {
    if((*it).ctxid== ctxid)
    {
      return (*it).ctxname;
    }
  }
  return QString::null;
}

void DBGFileInfo::clearStatus()
{
  m_statusUpdated = false;
}

bool  DBGFileInfo::updated()
{
  return m_statusUpdated;
}

void DBGFileInfo::clearModuleData()
{
  m_fileMap.clear();
}

void DBGFileInfo::clearContextData()
{
  m_contextList.clear();
}
