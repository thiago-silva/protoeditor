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

#ifndef EXECUTIONCONTROLLER_H
#define EXECUTIONCONTROLLER_H

#include <qobject.h>
#include <qstring.h>

class Variable;
class DebuggerBreakpoint;
class DebuggerExecutionPoint;
class DebuggerFactory;
class AbstractDebugger;

class ExecutionController : public QObject
{
  Q_OBJECT

public:

  ExecutionController();
  ~ExecutionController();

  void init();
  QString currentDebuggerName();

  //Protoeditor commands us

  void executeScript(const QString&, const QString&);

  void debugStart(const QString&, const QString&, bool);

  void profileScript(const QString&, const QString&, bool);

  void debugStop();

  void debugRunToCursor();

  void debugStepOver();
  void debugStepInto();
  void debugStepOut();  

  void modifyGlobalVariable(Variable*);
  void modifyLocalVariable(Variable*);
  void addWatch(const QString&);
  void removeWatch(Variable*);
  void addBreakpoint(DebuggerBreakpoint*);
  void changeBreakpoint(DebuggerBreakpoint*);
  void removeBreakpoint(DebuggerBreakpoint*);
  void changeCurrentExecutionPoint(DebuggerExecutionPoint*);

signals:
  void sigDebugStarted();
  void sigDebugEnded();

public slots:
  void slotDebugStarted(AbstractDebugger*);
  void slotDebugEnded();  
  void slotDebugPaused();
  void slotJITStarted(AbstractDebugger*);

  void slotNeedChildren(int, Variable*);

  void slotExecuteCmd(const QString&);
private:
  void sessionPrologue();
  bool checkForOpenedFile();
  bool debugPrologue(const QString&, bool);

  AbstractDebugger  *m_activeDebugger;
  DebuggerFactory   *m_debuggerFactory;
};

#endif
