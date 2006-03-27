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

QMap<QString, AbstractDebugger*> DebuggerFactory::buildDebuggers(DebuggerManager* manager) {
  QMap<QString, AbstractDebugger*> map;

  AbstractDebugger* debugger;

  //DBG
  debugger = new DebuggerDBG(manager);
  map[debugger->name()] = debugger;

  //Xdebug
  debugger = new DebuggerXD(manager);
  map[debugger->name()] = debugger;

  //Gubed
  debugger = new DebuggerGB(manager);
  map[debugger->name()] = debugger;

  return map;
}
