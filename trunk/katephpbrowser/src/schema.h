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
#ifndef SCHEMA_H
#define SCHEMA_H

#include <qstring.h>
#include <qvaluelist.h>
#include <kurl.h>

typedef QValueList<KURL> DirectoryList_t;
class Schema
{
public:
  Schema();
  Schema(const QString&, const DirectoryList_t&);
  Schema(const QString&, const QStringList&);
  ~Schema();

  void setName(const QString&);
  QString name() const;

  void addDirectory(const KURL&);
  void setDirectoryList(const DirectoryList_t&);
  const DirectoryList_t directoryList() const;
private:
  QString m_name;
  DirectoryList_t m_directoryList;
};

#endif
