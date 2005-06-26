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
#include <qmap.h>
#include <qstring.h>
#include "variable.h"

//class DebuggerConfigurations;
class DebuggerStack;
class AbstractDebugger;
class DebuggerBreakpoint;
class MainWindow;
class DebuggerExecutionPoint;

class DebuggerManager : public QObject
{
  Q_OBJECT
public:

  enum { InfoMsg, WarningMsg, ErrorMsg, }; //Debugger session message types (non-internal)

  DebuggerManager(MainWindow* window, QObject *parent = 0, const char* name = 0);
  ~DebuggerManager();

  void init();


  /* for debugger */
  void updateStack(DebuggerStack*);
  void updateGlobalVars(VariablesList_t*);
  void updateLocalVars(VariablesList_t*);
  void updateWatch(Variable*);
  void updateBreakpoint(DebuggerBreakpoint* bp);
  void debugMessage(int, const QString&, const QString&, int);
  void debugError(const QString&);
//   void updateOutput(const QString&);
  void addOutput(const QString&);

public slots:
  void slotConfigurationChanged();
  void slotGotoLineAtFile(const QString&, int);
  
private slots:
  /* Application - DebuggerManager */

  void slotDebugStart();
  void slotDebugStop();
  void slotDebugStepInto();
  void slotDebugStepOver();
  void slotDebugStepOut();
  void slotDebugToggleBp();

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

  void slotProfile();

  /* DebuggerClient - DebuggerManager */

  void slotDebugStarting();
  void slotDebugStarted(AbstractDebugger*);
  void slotDebugEnded();
  void slotDebugBreak();
  
  //Debugger internal error (conection, listen port, etc)
  void slotInternalError(const QString&);

//   void slotProfileDialogClosed();
private:
  void debugActiveScript();
  void debugCurrentSiteScript();
  void profileActiveScript();
  void profileCurrentSiteScript();
    
  void connectDebugger(AbstractDebugger*);

  void loadDebuggers();
  void clearDebuggers();

  QMap<QString, AbstractDebugger*> m_debuggerMap;
  AbstractDebugger* m_activeDebugger;
  MainWindow       *m_window;
//   bool             m_showProfileDialog;
};

#endif
