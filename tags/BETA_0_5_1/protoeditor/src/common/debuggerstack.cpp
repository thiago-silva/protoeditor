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

#include "debuggerstack.h"


DebuggerExecutionLine::DebuggerExecutionLine()
  : m_id(0), m_filePath(), m_line(0), m_function(0)
{
}

DebuggerExecutionLine::DebuggerExecutionLine(int id, QString filePath, int line, QString function)
  : m_id(id), m_filePath(filePath), m_line(line), m_function(function)
{
}

DebuggerExecutionLine::~DebuggerExecutionLine()
{
}

void DebuggerExecutionLine::setId(int id)
{
  m_id = id;
}

int DebuggerExecutionLine::id()
{
  return m_id;
}

void DebuggerExecutionLine::setFilePath(QString filePath)
{
  m_filePath = filePath;
}

QString DebuggerExecutionLine::filePath()
{
  return m_filePath;
}

void DebuggerExecutionLine::setLine(int line)
{
  m_line = line;
}

int DebuggerExecutionLine::line()
{
  return m_line;
}

void DebuggerExecutionLine::setFunction(QString function)
{
  m_function = function;
}

QString DebuggerExecutionLine::function()
{
  return m_function;
}

//-----------------------------------------------------------

DebuggerStack::DebuggerStack()
{
}

DebuggerStack::~DebuggerStack()
{
  DebuggerExecutionLine *execLine;
  for(execLine = m_execLineList.first(); execLine; execLine = m_execLineList.next()) {
    delete execLine;
  }
}

void DebuggerStack::push(int id, QString filePath, int line, QString function)
{
  DebuggerExecutionLine *execLine =
    new DebuggerExecutionLine(id, filePath, line, function);
  m_execLineList.prepend(execLine);
}

void DebuggerStack::push(DebuggerExecutionLine* execLine)
{
  m_execLineList.prepend(execLine);
}

DebuggerExecutionLine* DebuggerStack::topExecutionLine()
{
  return m_execLineList.getFirst();
}

DebuggerStack::DebuggerExecutionLineList_t DebuggerStack::debuggerExecutionLineList()
{
  return m_execLineList;
}
