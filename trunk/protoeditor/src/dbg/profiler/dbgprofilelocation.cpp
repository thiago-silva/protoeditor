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

#include "dbgprofilelocation.h"

DBGProfileLocation::DBGProfileLocation()
  : m_modId(0),
    m_ctxId(0),
    m_moduleName(""),
    m_contextName(""),
    m_line(0)
{}

DBGProfileLocation::DBGProfileLocation(int modId , const QString& moduleName, int ctxId, const QString& contextName, int line)
    : m_modId(modId),
    m_ctxId(ctxId),
    m_moduleName(moduleName),
    m_contextName(contextName),
    m_line(line)
{}

DBGProfileLocation::~DBGProfileLocation()
{}

void DBGProfileLocation::setModuleId(int id)
{
  m_modId = id;
}

int DBGProfileLocation::moduleId() const
{
  return m_modId;
}

void DBGProfileLocation::setContextId(int id)
{
  m_ctxId = id;
}

int DBGProfileLocation::contextId() const
{
  return m_ctxId;
}

void DBGProfileLocation::setModuleName(const QString& moduleName)
{
  m_moduleName = moduleName;
}

const QString& DBGProfileLocation::moduleName() const
{
  return m_moduleName;
}

void DBGProfileLocation::setContextName(const QString& contextName)
{
  m_contextName = contextName;
}

const QString DBGProfileLocation::contextName() const
{
  return m_contextName;
}

void DBGProfileLocation::setLine(int line)
{
  m_line = line;
}

int DBGProfileLocation::line() const
{
  return m_line;
}


