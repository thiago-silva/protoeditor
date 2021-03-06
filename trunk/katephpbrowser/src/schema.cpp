/***************************************************************************
 *   Copyright (C) 2006 by Thiago Silva                                    *
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
#include "schema.h"
#include <qstringlist.h>

Schema::Schema()
{
}

Schema::Schema(const QString& name, const DirectoryList_t& list)
  : m_name(name), m_directoryList(list)
{
}

Schema::Schema(const QString& name, const QStringList& dirs)
  : m_name(name)
{
  for(QStringList::const_iterator it = dirs.begin(); it != dirs.end(); ++it) 
  {
    m_directoryList.append(*it);
  }  
}

Schema::~Schema()
{
}

void Schema::setName(const QString& name)
{
  m_name = name;
}

QString Schema::name() const
{
  return m_name;
}

void Schema::addDirectory(const KURL& url)
{
  m_directoryList.append(url);
}

void Schema::setDirectoryList(const DirectoryList_t& list)
{
  m_directoryList = list;
}

const DirectoryList_t Schema::directoryList() const
{
  return m_directoryList;
}

void Schema::setFolderStructure(const QMap<QString, QStringList>& map)
{
  m_folderStructure = map;
}

QMap<QString, QStringList> Schema::folderStructure()
{
  return m_folderStructure;
}
