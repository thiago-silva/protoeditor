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

#ifndef DEBUGGERTABWIDGET_H
#define DEBUGGERTABWIDGET_H

#include <ktabwidget.h>

#include "variable.h"

class KURL;

class DebuggerBreakpoint;
class DebuggerStack;
class DebuggerExecutionPoint;


class VariableListView;
class LocalTab;
class WatchTab;
class BreakpointListView;
class MessageListView;
class KTextEdit;
class ConsoleWidget;

/* Facade class */

class DebuggerUI : public KTabWidget
{
  Q_OBJECT
public:
  enum { GlobalVarListID, LocalVarListID, WatchListID };

  DebuggerUI(QWidget* parent, const char *name = 0);
  ~DebuggerUI();

  //Global VariablesListView
  void setGlobalVariables(VariableList_t* vars);

  //Local VariablesListView
  void setLocalVariables(VariableList_t* vars);

  //all variables
  void updateVariable(Variable*, int);

  //ComboStack
  void setStack(DebuggerStack*);
  DebuggerStack* stack();
  DebuggerExecutionPoint* selectedDebuggerExecutionPoint();
  int currentStackItem();
 

  //WatchTab
  void addWatch(Variable*);
  QStringList watches();

  //BreakpointListView
  void updateBreakpoint(DebuggerBreakpoint*);
  void toggleBreakpoint(const KURL&, int, bool enabled = true);
  void resetBreakpointItems();
  QValueList<DebuggerBreakpoint*> breakpoints();
  QValueList<DebuggerBreakpoint*> breakpointsFrom(const KURL&);

  //MessageListview
  void addMessage(int, const QString&, int, const KURL&);

  //Output
  void appendOutput(const QString&);

  //Console
  void appendConsoleDebuggerText(const QString&);
  void appendConsoleUserText(const QString&);

  void prepareForSession();
  void cleanSession();

signals:
  //All variableslistview
  void sigNeedChildren(int, Variable*);

  //Global VariableListView
  void sigGlobalVarModified(Variable*);

  //Local VariableListView
  void sigLocalVarModified(Variable*);

  //ComboStack
  void sigStackchanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*);

  //Watch
  void sigWatchAdded(const QString&);
  void sigWatchModified(Variable*);
  void sigWatchRemoved(Variable*);

  //BreakpointListView
  void sigBreakpointCreated(DebuggerBreakpoint*);
  void sigBreakpointChanged(DebuggerBreakpoint*);
  void sigBreakpointRemoved(DebuggerBreakpoint*);

  //Console
  void sigExecuteCmd(const QString&);

  void sigGotoFileAndLine(const KURL&, int);

public slots:
  //BreakpointListView
  void slotBreakpointMarked(const KURL&, int, bool);
  void slotBreakpointUnmarked(const KURL&, int);

  void slotNeedChildren(int, Variable*);
private:
  VariableListView    *m_globalVariableListView;
  LocalTab            *m_localTab;
  WatchTab            *m_watchTab;
  BreakpointListView  *m_breakpointListView;
  MessageListView     *m_messageListView;  
  KTextEdit           *m_edOutput;
  ConsoleWidget       *m_console;
};

#endif
