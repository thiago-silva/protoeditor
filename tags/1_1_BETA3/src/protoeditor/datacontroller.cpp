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

#include "datacontroller.h"

#include "debuggerstack.h"
#include "debuggerbreakpoint.h"

#include "protoeditor.h"
#include "executioncontroller.h"
#include "uinterface.h"
#include "editorui.h"
#include "debuggerui.h"

DataController::DataController()
  : QObject()
{
}


DataController::~DataController()
{
}


void DataController::updateStack(DebuggerStack* stack)
{
  //**** dealing with the current stackCombo (MARKS stuff)
  //-if stackCombo isn't empty, unmark the previously marked ExecutionMark
  // (wich is the top context of the comboStack)
  //-if comboStack is not pointed to the top context, unmark the previously
  // marked PreExecutionPoint and request the vars for this context

  //Conveniece vars:
  DebuggerExecutionPoint* execPoint;
  EditorInterface* ed = Protoeditor::self()->uinterface()->editorUI();

  if(Protoeditor::self()->uinterface()->debuggerUI()->stack()->count() > 0)
  {
    execPoint =
      Protoeditor::self()->uinterface()->debuggerUI()->stack()->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }

  if(Protoeditor::self()->uinterface()->debuggerUI()->currentStackItem() != 0)
  {
    execPoint =
      Protoeditor::self()->uinterface()->debuggerUI()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }

  //**** dealing with the new Stack (the argument)
  //-set the current document/line according to the top context of the stack
  //-mark the new ExecutionPoint
  //-sets the stack to the comboStack

  //lets update first and force the user to be on the top of the stack
  Protoeditor::self()->uinterface()->debuggerUI()->setStack(stack);

  execPoint = stack->topExecutionPoint();
  ed->activateDocument(execPoint->url(), true);
  ed->gotoLineAtFile(execPoint->url(), execPoint->line()-1);

  ed->markExecutionPoint(execPoint->url(), execPoint->line());
}

void DataController::updateGlobalVars(VariableList_t* vars)
{
  Protoeditor::self()->uinterface()->debuggerUI()->setGlobalVariables(vars);
}

void DataController::updateLocalVars(VariableList_t* vars)
{
  Protoeditor::self()->uinterface()->debuggerUI()->setLocalVariables(vars);
}

void DataController::updateWatch(Variable* var)
{
  Protoeditor::self()->uinterface()->debuggerUI()->addWatch(var);
}

void DataController::updateVariable(Variable* var)
{
  Protoeditor::self()->uinterface()->debuggerUI()->updateVariable(var);
}

void DataController::updateBreakpoint(DebuggerBreakpoint* bp)
{
  Protoeditor::self()->uinterface()->debuggerUI()->updateBreakpoint(bp);

  switch(bp->status())
  {
    case DebuggerBreakpoint::ENABLED:
      Protoeditor::self()->uinterface()->editorUI()->unmarkDisabledBreakpoint(bp->url(), bp->line());
      Protoeditor::self()->uinterface()->editorUI()->markActiveBreakpoint(bp->url(), bp->line());
      break;
    case DebuggerBreakpoint::DISABLED:
      Protoeditor::self()->uinterface()->editorUI()->unmarkActiveBreakpoint(bp->url(), bp->line());
      Protoeditor::self()->uinterface()->editorUI()->markDisabledBreakpoint(bp->url(), bp->line());
    case DebuggerBreakpoint::UNRESOLVED:
      break;
  }
}

void DataController::debugMessage(int type, const QString& msg, const KURL& url, int line)
{
  if(type == DataController::ErrorMsg) 
  {
    Protoeditor::self()->uinterface()->editorUI()->gotoLineAtFile(url, line-1);
    Protoeditor::self()->showError(msg);
  }
  Protoeditor::self()->uinterface()->debuggerUI()->addMessage(type, msg, line, url);
}

void DataController::addOutput(const QString& output)
{
   Protoeditor::self()->uinterface()->debuggerUI()->appendOutput(output);
}

/***************** SLOTS **********************/


void DataController::slotDebugStarted()
{
  //removes any existing pre execution point
  EditorInterface* ed = Protoeditor::self()->uinterface()->editorUI();
  DebuggerStack* stack = Protoeditor::self()->uinterface()->debuggerUI()->stack();

  if(!stack->isEmpty())
  {
    DebuggerExecutionPoint* execPoint;
    execPoint =
      Protoeditor::self()->uinterface()->debuggerUI()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }
}

void DataController::slotDebugEnded()
{
  //removes the current execution point
  EditorInterface* ed = Protoeditor::self()->uinterface()->editorUI();
  DebuggerStack* stack = Protoeditor::self()->uinterface()->debuggerUI()->stack();

  if(!stack->isEmpty())
  {
    //remove the execution line mark
    DebuggerExecutionPoint* execPoint;
    execPoint = stack->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->url());
  }  
}

void DataController::slotNewDocument()
{
  //if the new document has breakpoints, mark them.
  EditorInterface* editor = Protoeditor::self()->uinterface()->editorUI();

  QValueList<DebuggerBreakpoint*> bplist =
    Protoeditor::self()->uinterface()->debuggerUI()->breakpointsFrom(
      editor->currentDocumentURL());

  QValueList<DebuggerBreakpoint*>::iterator it;
  for(it = bplist.begin(); it != bplist.end(); ++it)
  {
    Protoeditor::self()->uinterface()->editorUI()->markActiveBreakpoint((*it)->url(), (*it)->line());
  }
}

void DataController::slotGlobalVarModified(Variable* var)
{
  Protoeditor::self()->executionController()->modifyGlobalVariable(var);
}

void DataController::slotLocalVarModified(Variable* var)
{
  Protoeditor::self()->executionController()->modifyLocalVariable(var);
}

void DataController::slotStackChanged(DebuggerExecutionPoint* old, DebuggerExecutionPoint* nw)
{
  //Glossary:
  //  -PreExecutionPoint: a DebuggerExecutionPoint representing a point in the stack of the backtrace.
  //                      This is used when the user selects a point on the stack ComboBox that is not the
  //                      topmost (the active line the debugger is running)
  
  
  //-set the current document/line according to the new stack context
  //-unmark the (possibly) previously PreExecutionPoint according to the old stack context
  //-mark the PreExecutionPoint of the new stack context
  //-request the variables for this context

  EditorInterface* ed = Protoeditor::self()->uinterface()->editorUI();

  ed->gotoLineAtFile(nw->url(), nw->line()-1);

  ed->unmarkPreExecutionPoint(old->url()/*, old->line()*/);

  if(nw != Protoeditor::self()->uinterface()->debuggerUI()->stack()->topExecutionPoint())
  {
    ed->markPreExecutionPoint(nw->url(), nw->line());
  }

  Protoeditor::self()->executionController()->changeCurrentExecutionPoint(nw);
}

void DataController::slotWatchAdded(const QString& expression)
{
  Variable* var = new Variable(expression);
  VariableValue* value = new VariableScalarValue(var);
  var->setValue(value); 
  Protoeditor::self()->debuggerUI()->addWatch(var);
  Protoeditor::self()->executionController()->addWatch(expression);
}

void DataController::slotWatchRemoved(Variable* var)
{
  Protoeditor::self()->executionController()->removeWatch(var);
}

void DataController::slotBreakpointCreated(DebuggerBreakpoint* bp)
{
  Protoeditor::self()->executionController()->addBreakpoint(bp);
}

void DataController::slotBreakpointChanged(DebuggerBreakpoint* bp)
{
  Protoeditor::self()->executionController()->changeBreakpoint(bp);

  switch(bp->status())
  {
    case DebuggerBreakpoint::ENABLED:
      Protoeditor::self()->uinterface()->editorUI()->unmarkDisabledBreakpoint(bp->url(), bp->line());
      Protoeditor::self()->uinterface()->editorUI()->markActiveBreakpoint(bp->url(), bp->line());
      break;
    case DebuggerBreakpoint::DISABLED:
      Protoeditor::self()->uinterface()->editorUI()->unmarkActiveBreakpoint(bp->url(), bp->line());
      Protoeditor::self()->uinterface()->editorUI()->markDisabledBreakpoint(bp->url(), bp->line());
    case DebuggerBreakpoint::UNRESOLVED:
      break;
  }
}

void DataController::slotBreakpointRemoved(DebuggerBreakpoint* bp)
{
  if(bp->status() == DebuggerBreakpoint::ENABLED) {
    Protoeditor::self()->uinterface()->editorUI()->unmarkActiveBreakpoint(
      bp->url(), bp->line());
  } else {
    Protoeditor::self()->uinterface()->editorUI()->unmarkDisabledBreakpoint(
      bp->url(), bp->line());
  }

  Protoeditor::self()->executionController()->removeBreakpoint(bp);
}

void DataController::slotConsoleDebuggerOutput(const QString& str)
{
  Protoeditor::self()->debuggerUI()->appendConsoleDebuggerText(str);
}

void DataController::slotConsoleUserOutput(const QString& str)
{
  Protoeditor::self()->debuggerUI()->appendConsoleUserText(str);
}
#include "datacontroller.moc"
