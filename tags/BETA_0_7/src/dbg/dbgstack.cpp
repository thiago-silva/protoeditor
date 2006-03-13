/***************************************************************************
 *   Copyright (C) 2004-2005 by Thiago Silva                                    *
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
#include "dbgstack.h"
#include "debuggerstack.h"
#include "dbgnetdata.h"
#include "dbgfileinfo.h"

DBGStack::DBGStack()
    : /*m_stack(NULL), */ m_frozen(false)
{}

DBGStack::~DBGStack()
{}

void DBGStack::add(dbgint modno, const QString& descr, dbgint lineno, dbgint scopeid)
{
  if(m_frozen) return;

  StackInfo info;
  info.line = lineno;
  info.function = descr;
  info.modNo = modno;
  info.scopeId = scopeid;
  m_stackInfoList.push_front(info);
}

void DBGStack::clear()
{
  m_frozen = false;
  m_stackInfoList.clear();
  //m_stack = NULL;
}

bool DBGStack::isEmpty()
{
  return m_stackInfoList.isEmpty();
}

DebuggerStack* DBGStack::debuggerStack(DBGFileInfo* dbgFileInfo)
{
  DebuggerStack* stack = new DebuggerStack();

  QString function;

  QValueList<StackInfo>::iterator it;

  for(it = m_stackInfoList.begin(); it != m_stackInfoList.end(); ++it) {
    if(((*it).function).find("::") == -1) {
      function = (*it).function;
    } else {
      function = dbgFileInfo->moduleName((*it).modNo) + "::main()";
      //function = "main()";
    }
    stack->push((*it).scopeId, dbgFileInfo->moduleName((*it).modNo), (*it).line, function);
  }

  return stack;
}

void DBGStack::freeze()
{
  m_frozen = true;
}

bool DBGStack::isFrozen()
{
  return m_frozen;
}
