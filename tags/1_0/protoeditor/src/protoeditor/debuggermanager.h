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

#ifndef DEBUGGERMANAGER_H
#define DEBUGGERMANAGER_H
#include <qobject.h>
#include <qmap.h>
#include <qstring.h>
#include "variable.h"

class DebuggerStack;
class AbstractDebugger;
class DebuggerBreakpoint;
class MainWindow;
class DebuggerExecutionPoint;
class SiteSettings;
class KURL;

/*
 The core of the application.
 Most of the data flow is represented in this class.
 The other part is the implementation-dependet handled by individual debugger clients.
*/

class DebuggerManager : public QObject
{
  Q_OBJECT
public:

  enum { InfoMsg, WarningMsg, ErrorMsg, }; //Debugger session message types (non-internal)

  DebuggerManager(MainWindow* window, QObject *parent = 0, const char* name = 0);
  ~DebuggerManager();

  void init();

  /* for debugger to notify us */
  void updateStack(DebuggerStack*);
  void updateGlobalVars(VariablesList_t*);
  void updateLocalVars(VariablesList_t*);
  void updateWatch(Variable*);
  void updateBreakpoint(DebuggerBreakpoint* bp);
  void debugMessage(int, const QString&, const KURL&, int);
//   void updateOutput(const QString&);
  void addOutput(const QString&);

public slots:
  void slotConfigurationChanged();
  void slotGotoLineAtFile(const KURL&, int);

  /* Application (Window) to DebuggerManager
     Those are called when the user manipulates the UI.
  */
  void slotScriptRun();
  void slotDebugStart();
  
  void slotProfile();

  void slotDebugStop();
  void slotDebugStepInto();
  void slotDebugStepOver();
  void slotDebugStepOut();
  void slotDebugToggleBp();
  void slotDebugRunToCursor();

private slots:

  void slotAddWatch(); //from WatchList
  void slotAddWatch(const QString& expression); //from editor ctx menu
  void slotComboStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*);
  void slotGlobalVarModified(Variable* var);
  void slotLocalVarModified(Variable* var);
  void slotBreakpointCreated(DebuggerBreakpoint*);
  void slotBreakpointChanged(DebuggerBreakpoint*);
  void slotBreakpointRemoved(DebuggerBreakpoint*);
  void slotWatchRemoved(Variable*);
  void slotNoDocument();
  
  void slotNewDocument();  

  /* AbstractDebugger to DebuggerManager */

  void slotDebugStarting();
  void slotDebugStarted(AbstractDebugger*);
  void slotDebugEnded();
  void slotDebugPaused();
  
  void slotJITStarted(AbstractDebugger*);

  //Debugger internal error (conection, listen port, etc)
  void slotError(const QString&);

private:
  KURL sessionPrologue(bool isProfiling);
  void processSession(const KURL& url, bool local, bool isProfiling);
    
  void connectDebugger(AbstractDebugger*);

  void loadDebuggers();
  void clearDebuggers();

  QMap<QString, AbstractDebugger*> m_debuggerMap;
  AbstractDebugger* m_activeDebugger;
  MainWindow       *m_window;
};

#endif
