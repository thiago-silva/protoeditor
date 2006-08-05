#include "debuggerinterface.h"

#include <ktextedit.h>

#include "variablelistview.h"
#include "localtab.h"
#include "watchtab.h"
#include "breakpointlistview.h"
#include "messagelistview.h"
#include "watchlistview.h"
#include "combostack.h"
#include "consolewidget.h"
#include "debuggerstack.h"


DebuggerInterface::~DebuggerInterface()
{
}

//Global VariablesListView
void DebuggerInterface::setGlobalVariables(VariableList_t* vars)
{
  m_globalVariableListView->setVariables(vars);
}

//Local VariablesListView
void DebuggerInterface::setLocalVariables(VariableList_t* vars)
{
  m_localTab->localVarList()->setVariables(vars);
}

void DebuggerInterface::updateVariable(Variable* var)
{
  int varlistID = m_varlistIDs.first();
  m_varlistIDs.remove(varlistID);

  switch(varlistID)
  {
    case GlobalVarListID:
      m_globalVariableListView->updateVariable(var);
      break;
    case LocalVarListID:
      m_localTab->localVarList()->updateVariable(var);
      break;
    case WatchListID:
      m_watchTab->watchListView()->updateVariable(var);
      break;
    default:
      break;
  } 
}

//ComboStack
void DebuggerInterface::setStack(DebuggerStack* stack)
{
  m_localTab->comboStack()->setStack(stack);
}

DebuggerStack* DebuggerInterface::stack()
{
  return m_localTab->comboStack()->stack();
}

DebuggerExecutionPoint* DebuggerInterface::selectedDebuggerExecutionPoint()
{
  return m_localTab->comboStack()->selectedDebuggerExecutionPoint();
}

int DebuggerInterface::currentStackItem()
{
  return m_localTab->comboStack()->currentItem();
}

//WatchTab
void DebuggerInterface::addWatch(Variable* var)
{
  m_watchTab->watchListView()->addWatch(var);
}

QStringList DebuggerInterface::watches()
{
  return m_watchTab->watchListView()->watches();
}

//BreakpointListView
void DebuggerInterface::updateBreakpoint(DebuggerBreakpoint* bp)
{
  m_breakpointListView->updateBreakpoint(bp);
}

void DebuggerInterface::toggleBreakpoint(const KURL& url, int line, bool enabled)
{
  m_breakpointListView->toggleBreakpoint(url, line, enabled);
}

void DebuggerInterface::resetBreakpointItems()
{
  m_breakpointListView->resetBreakpointItems();
}

QValueList<DebuggerBreakpoint*> DebuggerInterface::breakpoints()
{
  return m_breakpointListView->breakpoints();
}

QValueList<DebuggerBreakpoint*> DebuggerInterface::breakpointsFrom(const KURL& url)
{
  return m_breakpointListView->breakpointsFrom(url);
}

//MessageListview
void DebuggerInterface::addMessage(int type, const QString& message, int line, const KURL& url)
{
  m_messageListView->add(type, message, line, url);
}

//Output
void DebuggerInterface::appendOutput(const QString& str)
{
  m_edOutput->append(str);
}

//Console
void DebuggerInterface::appendConsoleDebuggerText(const QString&)
{
//   m_console->appendDebuggerText(str);
}

void DebuggerInterface::appendConsoleUserText(const QString&)
{
//   m_console->appendUserText(str);
}

void DebuggerInterface::prepareForSession()
{
  m_globalVariableListView->setReadOnly(false);
  m_localTab->localVarList()->setReadOnly(false);
  m_watchTab->watchListView()->setReadOnly(false);
  m_watchTab->watchListView()->reset();

  m_globalVariableListView->clear();
  m_localTab->localVarList()->clear();
  m_messageListView->clear();
  m_localTab->comboStack()->clear();
  m_edOutput->clear();
}

void DebuggerInterface::cleanSession()
{
  m_breakpointListView->resetBreakpointItems();

  //do not let the user change variables anymore
  m_globalVariableListView->setReadOnly(true);
  m_localTab->localVarList()->setReadOnly(true);
  m_watchTab->watchListView()->setReadOnly(true);
}
