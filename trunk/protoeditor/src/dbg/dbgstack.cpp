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
#include "dbgstack.h"
#include "debuggerstack.h"

DBGStack::DBGStack()
  : m_ready(false)
{}

DBGStack::~DBGStack()
{}

void DBGStack::add(dbgint modno, QString descr, dbgint lineno, dbgint scopeid)
{
  StackInfo info;
  info.line = lineno;
  info.function = descr;
  info.modNo = modno;
  info.scopeId = scopeid;
  m_stackInfoList.push_front(info);
}

void DBGStack::setModulePath(dbgint modno, QString filePath)
{
  QValueList<StackInfo>::iterator it;
  for(it = m_stackInfoList.begin(); it != m_stackInfoList.end(); ++it)
  {
    if((*it).modNo == modno)
    {
      (*it).filePath = filePath;
      //break; do not break. We may have many execpoints on a single file
    }
  }

  //now that we are setting the filepaths correctly (from the srctree request),
  //we can build the stack
  m_ready = true;
}

void DBGStack::clear()
{
  m_ready = false;
  m_stackInfoList.clear();
}

bool DBGStack::ready()
{
  return m_ready;
}

DebuggerStack* DBGStack::debuggerStack()
{
  DebuggerStack* stack = new DebuggerStack();

  QValueList<StackInfo>::iterator it;
  for(it = m_stackInfoList.begin(); it != m_stackInfoList.end(); ++it)
  {
    stack->push((*it).scopeId, (*it).filePath, (*it).line, (*it).function);
  }

  //be ready for the next
  clear();

  return stack;
}
