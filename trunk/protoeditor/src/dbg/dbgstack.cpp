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
#include "dbgtags.h"
#include "dbgfileinfo.h"

DBGStack::DBGStack()
  :   m_stack(0)
{
}

DBGStack::~DBGStack()
{
}

void DBGStack::setStackTagList(DBGResponsePack::StackTagList_t list)
{

  DBGResponsePack::StackTagPair_t p;
  DBGResponseTagStack* stackTag;
  DBGTagRawdata* rawTag;

  DBGResponsePack::StackTagList_t::iterator it;
  for(it = list.begin(); it != list.end(); ++it) {
    stackTag = (*it).first;
    rawTag = (*it).second;

    StackInfo info;
    info.line = stackTag->lineNo();
    info.function = rawTag->data();
    info.modNo = stackTag->modNo();
    info.scopeId = stackTag->scopeId();
    m_stackInfoList.push_front(info);
  }
}

void DBGStack::clear()
{
  m_stackInfoList.clear();
}

DebuggerStack* DBGStack::debuggerStack(DBGFileInfo* dbgFileInfo)
{
  if(m_stack) {
    return m_stack;
  }

  DebuggerStack* m_stack = new DebuggerStack();

  QString function;

  QValueList<StackInfo>::iterator it;

  for(it = m_stackInfoList.begin(); it != m_stackInfoList.end(); ++it) {
    if(((*it).function).find("::") == -1) {
      function = (*it).function;
    } else {
      function = "main()";
    }
    m_stack->push((*it).scopeId, dbgFileInfo->localFilePath((*it).modNo), (*it).line, function);
  }
  return m_stack;
}
