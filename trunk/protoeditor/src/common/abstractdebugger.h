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

#ifndef ABSTRACTDEBUGGER_H
#define ABSTRACTDEBUGGER_H

#include <variable.h>
#include <qobject.h>
#include <qstring.h>

class DebuggerManager;
class DebuggerBreakpoint;
class DebuggerConfigurations;
class DebuggerExecutionLine;
class DebuggerStack;

class AbstractDebugger : public QObject {
Q_OBJECT
public:
  AbstractDebugger(DebuggerManager* manager, QObject *parent = 0, const char* name = 0);

  AbstractDebugger(AbstractDebugger*); //copy ctor

  virtual ~AbstractDebugger();

  int id();
  DebuggerManager* debuggerManager();

  bool isSessionActive();
  bool isRunning();

  virtual void startSession() = 0;
  virtual void endSession()   = 0;
  virtual void run(QString)   = 0;
  virtual void stop()         = 0;
  virtual void stepInto()     = 0;
  virtual void stepOver()     = 0;
  virtual void stepOut()      = 0;

  virtual QString name()      = 0;
  virtual void loadConfiguration(DebuggerConfigurations*) = 0;

  virtual void addBreakpoints(QPtrList<DebuggerBreakpoint>) = 0;
  virtual void addBreakpoint(DebuggerBreakpoint*) = 0;
  //virtual void addBreakpoint(QString, int) = 0;
  virtual void changeBreakpoint(DebuggerBreakpoint*) = 0;
  virtual void removeBreakpoint(DebuggerBreakpoint*) = 0;
  //virtual void requestBreakpointList() = 0;

  virtual void requestLocalVariables(DebuggerExecutionLine* stackContext) = 0;
  virtual void modifyVariable(Variable* v, DebuggerExecutionLine* stackContext) = 0;

  //default stack context: global variables
  virtual void addWatch(QString expression, DebuggerExecutionLine* stackContext = 0) = 0;
  virtual void removeWatch(QString expression) = 0;
  virtual void requestWatches(DebuggerExecutionLine* stackContext) = 0;

signals:

  void sigSessionStarted();
  void sigSessionEnded();

  void sigDebugStarted();
  void sigDebugEnded();
  void sigVariablesChanged(VariablesList_t*, bool);
  void sigStackChanged(DebuggerStack*);
  void sigWatchChanged(Variable*);
  void sigBreakpointChanged(DebuggerBreakpoint*);

  //TODO: rename to sigMessage, sigOutput, sigError.

  void sigDebugMessage(int, QString, int, QString);
  void sigDebugOutput(QString);
  void sigDebugError(QString); //Session debug error
  void sigInternalError(QString);//Debugger internal error (conection, listen port, etc)


protected:
  void setId(int);
  void setRunning(bool);
  void setSessionActive(bool);

private:
  int m_id;

  bool m_isSessionActive;
  bool m_isRunning;

  DebuggerManager* m_manager;
};

#endif
