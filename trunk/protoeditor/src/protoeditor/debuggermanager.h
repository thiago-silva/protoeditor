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

#ifndef DEBUGGERMANAGER_H
#define DEBUGGERMANAGER_H
#include <qobject.h>
#include <qstring.h>
#include "variable.h"

//class DebuggerConfigurations;
class DebuggerStack;
class AbstractDebugger;
class DebuggerBreakpoint;
class MainWindow;
class DebuggerExecutionLine;

class DebuggerManager : public QObject {
Q_OBJECT
public:

  enum { InfoMsg, WarningMsg, ErrorMsg, }; //Debugger session message types (non-internal)

  DebuggerManager(MainWindow* window, QObject *parent = 0, const char* name = 0);
  ~DebuggerManager();

  void init();

public slots:
  void slotConfigurationChanged();

private slots:
  /* Application - DebuggerManager */

  void slotDebugStartSession();
  void slotDebugEndSession();
  void slotDebugRun();
  void slotDebugStop();
  void slotDebugStepInto();
  void slotDebugStepOver();
  void slotDebugStepOut();
  void slotDebugToggleBp();

  void slotAddWatch();
  void slotComboStackChanged(DebuggerExecutionLine*, DebuggerExecutionLine*);
  void slotVarModified(Variable* var);
  void slotBreakpointCreated(DebuggerBreakpoint*);
  void slotBreakpointChanged(DebuggerBreakpoint*);
  void slotBreakpointRemoved(DebuggerBreakpoint*);
  //void slotBreakpointDeleted(DebuggerBreakpoint*);
  void slotWatchRemoved(Variable*);

  /* DebuggerClient - DebuggerManager */

  //TODO: set prefix to all: slotDebug*().
  //reason: there is an amniguious func: slotBreakpointCreated()
  //the user can change it and the debugger can change it

  void slotSessionStarted();
  void slotSessionEnded();
  void slotDebugStarted();
  void slotDebugEnded();
  void slotVariablesChanged(VariablesList_t*, bool);
  void slotWatchChanged(Variable*);
  void slotStackChanged(DebuggerStack*);
  void slotDebugBreakpointChanged(DebuggerBreakpoint*);
  void slotDebugMessage(int, QString, int, QString);
  void slotDebugOutput(QString);
  void slotDebugError(QString);    //Session debug error
  void slotInternalError(QString); //Debugger internal error (conection, listen port, etc)

private:
  //disable all
  void disableAllDebugActions();
  void connectDebugger();

  void loadDebugger();
  void clearDebugger();

  AbstractDebugger* m_debugger;
  MainWindow* m_window;
};

#endif
