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

#include "dbgfileinfo.h"
#include "debuggerstack.h"
#include "dbgtags.h"
#include "dbgconfiguration.h"

DBGFileInfo::DBGFileInfo(DBGConfiguration* conf)
  : m_configuration(conf)
{
}


DBGFileInfo::~DBGFileInfo()
{
}

void DBGFileInfo::loadFilePathInformation(DBGResponsePack::TreeStackList_t treeList)
{
  m_fileMap.clear();

  DBGResponsePack::TreeStackList_t::iterator it;
  for(it = treeList.begin(); it != treeList.end(); ++it)
  {
    m_fileMap[(*it).first->modNo()] = (*it).second->data();
  }
}

QString DBGFileInfo::localFilePath(int modno) {
  QString remotefile = m_fileMap[modno];

  remotefile.remove(0, m_configuration->serverBaseDir().length());
  QString ret = m_configuration->localBaseDir();
  ret.append(remotefile);
  return ret;
}

int DBGFileInfo::moduleNumber(QString remoteFilePath) {
  QMap<int, QString>::Iterator it;
  for( it = m_fileMap.begin(); it != m_fileMap.end(); ++it ) {
    if(it.data() == remoteFilePath) {
      return it.key();
    }
  }

  return 0;
}

void DBGFileInfo::setConfiguration(DBGConfiguration* conf) {
  m_configuration = conf;
}

void DBGFileInfo::clear() {
  m_fileMap.clear();
}
