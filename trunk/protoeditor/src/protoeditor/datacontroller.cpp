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
#include "mainwindow.h"
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
  EditorUI* ed = Protoeditor::self()->mainWindow()->editorUI();

  if(Protoeditor::self()->mainWindow()->debuggerUI()->stack()->count() > 0)
  {
    execPoint =
      Protoeditor::self()->mainWindow()->debuggerUI()->stack()->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }

  if(Protoeditor::self()->mainWindow()->debuggerUI()->currentStackItem() != 0)
  {
    execPoint =
      Protoeditor::self()->mainWindow()->debuggerUI()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }

  //**** dealing with the new Stack (the argument)
  //-set the current document/line according to the top context of the stack
  //-mark the new ExecutionPoint
  //-sets the stack to the comboStack

  //lets update first and force the user to be on the top of the stack
  Protoeditor::self()->mainWindow()->debuggerUI()->setStack(stack);

  execPoint = stack->topExecutionPoint();
  ed->setCurrentDocumentTab(execPoint->url(), true);
  ed->gotoLineAtFile(execPoint->url(), execPoint->line()-1);

  ed->markExecutionPoint(execPoint->url(), execPoint->line());
}

void DataController::updateGlobalVars(VariableList_t* vars)
{
  Protoeditor::self()->mainWindow()->debuggerUI()->setGlobalVariables(vars);
}

void DataController::updateLocalVars(VariableList_t* vars)
{
  Protoeditor::self()->mainWindow()->debuggerUI()->setLocalVariables(vars);
}

void DataController::updateWatch(Variable* var)
{
  Protoeditor::self()->mainWindow()->debuggerUI()->addWatch(var);
}

void DataController::updateBreakpoint(DebuggerBreakpoint* bp)
{
  Protoeditor::self()->mainWindow()->debuggerUI()->updateBreakpoint(bp);

  switch(bp->status())
  {
    case DebuggerBreakpoint::ENABLED:
      Protoeditor::self()->mainWindow()->editorUI()->unmarkDisabledBreakpoint(bp->url(), bp->line());
      Protoeditor::self()->mainWindow()->editorUI()->markActiveBreakpoint(bp->url(), bp->line());
      break;
    case DebuggerBreakpoint::DISABLED:
      Protoeditor::self()->mainWindow()->editorUI()->unmarkActiveBreakpoint(bp->url(), bp->line());
      Protoeditor::self()->mainWindow()->editorUI()->markDisabledBreakpoint(bp->url(), bp->line());
    case DebuggerBreakpoint::UNRESOLVED:
      break;
  }
}

void DataController::debugMessage(int type, const QString& msg, const KURL& url, int line)
{
  if(type == DataController::ErrorMsg) 
  {
    Protoeditor::self()->mainWindow()->editorUI()->gotoLineAtFile(url, line);
    Protoeditor::self()->mainWindow()->showError(msg);
  }
  Protoeditor::self()->mainWindow()->debuggerUI()->addMessage(type, msg, line, url);
}

void DataController::addOutput(const QString& output)
{
   Protoeditor::self()->mainWindow()->debuggerUI()->appendOutput(output);
}

/***************** SLOTS **********************/


void DataController::slotDebugStarted()
{
  //removes any existing pre execution point
  EditorUI* ed = Protoeditor::self()->mainWindow()->editorUI();
  DebuggerStack* stack = Protoeditor::self()->mainWindow()->debuggerUI()->stack();

  if(!stack->isEmpty())
  {
    DebuggerExecutionPoint* execPoint;
    execPoint =
      Protoeditor::self()->mainWindow()->debuggerUI()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }
}

void DataController::slotDebugEnded()
{
  //removes the current execution point
  EditorUI* ed = Protoeditor::self()->mainWindow()->editorUI();
  DebuggerStack* stack = Protoeditor::self()->mainWindow()->debuggerUI()->stack();

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
  QValueList<DebuggerBreakpoint*> bplist =
    Protoeditor::self()->mainWindow()->debuggerUI()->breakpointsFrom(
      Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL());

  QValueList<DebuggerBreakpoint*>::iterator it;
  for(it = bplist.begin(); it != bplist.end(); ++it)
  {
    Protoeditor::self()->mainWindow()->editorUI()->markActiveBreakpoint((*it)->url(), (*it)->line());
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

  EditorUI* ed = Protoeditor::self()->mainWindow()->editorUI();

  ed->gotoLineAtFile(nw->url(), nw->line()-1);

  ed->unmarkPreExecutionPoint(old->url()/*, old->line()*/);

  if(nw != Protoeditor::self()->mainWindow()->debuggerUI()->stack()->topExecutionPoint())
  {
    ed->markPreExecutionPoint(nw->url(), nw->line());
  }

  Protoeditor::self()->executionController()->changeCurrentExecutionPoint(nw);
}

void DataController::slotWatchAdded(const QString& expression)
{
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
      Protoeditor::self()->mainWindow()->editorUI()->unmarkDisabledBreakpoint(bp->url(), bp->line());
      Protoeditor::self()->mainWindow()->editorUI()->markActiveBreakpoint(bp->url(), bp->line());
      break;
    case DebuggerBreakpoint::DISABLED:
      Protoeditor::self()->mainWindow()->editorUI()->unmarkActiveBreakpoint(bp->url(), bp->line());
      Protoeditor::self()->mainWindow()->editorUI()->markDisabledBreakpoint(bp->url(), bp->line());
    case DebuggerBreakpoint::UNRESOLVED:
      break;
  }
}

void DataController::slotBreakpointRemoved(DebuggerBreakpoint* bp)
{
  if(bp->status() == DebuggerBreakpoint::ENABLED) {
    Protoeditor::self()->mainWindow()->editorUI()->unmarkActiveBreakpoint(
      bp->url(), bp->line());
  } else {
    Protoeditor::self()->mainWindow()->editorUI()->unmarkDisabledBreakpoint(
      bp->url(), bp->line());
  }

  Protoeditor::self()->executionController()->removeBreakpoint(bp);
}


#include "datacontroller.moc"
