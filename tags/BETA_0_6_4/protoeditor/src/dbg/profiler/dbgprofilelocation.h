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

#ifndef DBGPROFILELOCATION_H
#define DBGPROFILELOCATION_H

#include <qstring.h>

class DBGProfileLocation
{
public:
  DBGProfileLocation();
  DBGProfileLocation(int modId , const QString& moduleName, int ctxId, const QString& contextName, int line);
  ~DBGProfileLocation();

  void setModuleId(int);
  int moduleId() const;

  void setContextId(int);
  int contextId() const;

  void setModuleName(const QString&);
  const QString& moduleName() const;

  void setContextName(const QString&);
  const QString contextName() const;

  void setLine(int);
  int line() const;
private:
  int m_modId;
  int m_ctxId;
  QString m_moduleName;
  QString m_contextName;
  int m_line;
};

#endif
