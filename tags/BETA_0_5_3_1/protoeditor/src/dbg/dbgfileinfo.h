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
#ifndef DBGFILEINFO_H
#define DBGFILEINFO_H
#include <qmap.h>

class DBGConfiguration;

class DBGFileInfo {
public:
  DBGFileInfo(DBGConfiguration*);
  ~DBGFileInfo();

  QString toURI(const QString&);

  QString toRemoteFilePath(const QString&);
  QString toLocalFilePath(const QString&);

  const QString& moduleName(int modno);
  int moduleNumber(const QString&);

  void setModulePath(int, const QString&);

  bool updated();

  void clear();
  void clearStatus();

private:

  DBGConfiguration  *m_dbgConfiguration;
  bool               m_statusUpdated;
  QMap<int, QString> m_fileMap;
};

#endif
