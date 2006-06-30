/***************************************************************************
 *   Copyright (C) 2004-2006 by Thiago Silva                               *
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

#include "debuggerfactory.h"
#include "debuggerdbg.h"
#include "debuggerxd.h"
#include "debuggergb.h"
#include "perldebugger.h"


DebuggerFactory::DebuggerFactory()
{
  AbstractDebugger* debugger;

  //PHP/DBG
  debugger = new DebuggerDBG();
  registerDebugger(debugger);

  //PHP/Xdebug
  debugger = new DebuggerXD();
  registerDebugger(debugger);

  //PHP/Gubed
  debugger = new DebuggerGB();
  registerDebugger(debugger);

  //Perl Local
  debugger = new PerlDebugger();
  registerDebugger(debugger);
}


DebuggerFactory::~DebuggerFactory()
{
  QMap<QString, AbstractDebugger*>::iterator it;
  for(it = m_debuggerMap.begin(); it != m_debuggerMap.end(); ++it) {
     delete it.data();
  }
}

void DebuggerFactory::registerDebugger(AbstractDebugger* debugger)
{
  m_debuggerMap[debugger->name()] = debugger;
}

AbstractDebugger* DebuggerFactory::getDebugger(const QString& name)
{
  if(m_debuggerMap.contains(name))
  {
    return m_debuggerMap[name];
  }
  return NULL;
}
