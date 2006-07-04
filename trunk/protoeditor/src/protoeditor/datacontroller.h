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

#ifndef DATACONTROLLER_H
#define DATACONTROLLER_H

#include <qobject.h>

#include "variable.h"

class QString;
class KURL;

class DebuggerStack;
class DebuggerExecutionPoint;
class DebuggerBreakpoint;

class DataController : public QObject
{
  Q_OBJECT
public:
  enum { InfoMsg, WarningMsg, ErrorMsg, }; //Debugger session message types (non-internal)

  DataController();
  ~DataController();

  void updateStack(DebuggerStack*);
  void updateGlobalVars(VariableList_t*);
  void updateLocalVars(VariableList_t*);
  void updateWatch(Variable*);
  void updateVariable(Variable*, int);
  void updateBreakpoint(DebuggerBreakpoint*);
  void debugMessage(int, const QString&, const KURL&, int);
  void addOutput(const QString&);

public slots:
  void slotDebugStarted();
  void slotDebugEnded();
  void slotNewDocument();

  void slotGlobalVarModified(Variable*);
  void slotLocalVarModified(Variable*);
  void slotStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*);

  void slotWatchAdded(const QString&);
  void slotWatchRemoved(Variable*);

  void slotBreakpointCreated(DebuggerBreakpoint*);
  void slotBreakpointChanged(DebuggerBreakpoint*);
  void slotBreakpointRemoved(DebuggerBreakpoint*);

  void slotConsoleDebuggerOutput(const QString&);
  void slotConsoleUserOutput(const QString&);
};

#endif
