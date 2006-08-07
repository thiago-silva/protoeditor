#ifndef DEBUGGERINTERFACE_H
#define DEBUGGERINTERFACE_H

#include <qvaluelist.h>
#include <qstringlist.h>
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

class DebuggerInterface
{
public:
  enum { GlobalVarListID, LocalVarListID, WatchListID };

  ~DebuggerInterface();

  //Global VariablesListView
  void setGlobalVariables(VariableList_t* vars);

  //Local VariablesListView
  void setLocalVariables(VariableList_t* vars);

  //all variables
  void updateVariable(Variable*);

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

protected:
  void resetBreakpointItems();

  VariableListView    *m_globalVariableListView;
  LocalTab            *m_localTab;
  WatchTab            *m_watchTab;
  BreakpointListView  *m_breakpointListView;
  MessageListView     *m_messageListView;  
  KTextEdit           *m_edOutput;
//   ConsoleWidget       *m_console;

  QValueList<int> m_varlistIDs;
};

#endif
