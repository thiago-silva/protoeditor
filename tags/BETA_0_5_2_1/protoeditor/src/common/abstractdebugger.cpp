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

#include "abstractdebugger.h"
#include "debuggermanager.h"

AbstractDebugger::AbstractDebugger(DebuggerManager* manager, QObject *parent, const char* name)
  : QObject(parent, name), m_id(-1), m_isSessionActive(false), m_isRunning(false)
  , m_manager(manager)
{
}

AbstractDebugger::AbstractDebugger(AbstractDebugger* debugger)
  : m_id(debugger->m_id), m_manager(debugger->m_manager)
{
}

AbstractDebugger::~AbstractDebugger()
{
}

void AbstractDebugger::setId(int id)
{
  m_id = id;
}

int AbstractDebugger::id()
{
  return m_id;
}

DebuggerManager* AbstractDebugger::debuggerManager()
{
  return m_manager;
}

bool AbstractDebugger::isSessionActive()
{
  return m_isSessionActive;
}

bool AbstractDebugger::isRunning()
{
  return m_isRunning;
}

void AbstractDebugger::setRunning(bool value)
{
  m_isRunning = value;
}

void AbstractDebugger::setSessionActive(bool value)
{
  m_isSessionActive = value;
}

#include "abstractdebugger.moc"
