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

#include "debuggermanager.h"
#include "mainwindow.h"
#include "debuggerstack.h"
#include "editortabwidget.h"
#include "abstractdebugger.h"
#include "debuggerfactory.h"
#include "debuggercombostack.h"
#include "variableslistview.h"
#include "watchlistview.h"
#include "loglistview.h"
#include "debuggerbreakpoint.h"
#include "breakpointlistview.h"
#include "debuggersettings.h"

#include <kapplication.h>
#include <kcombobox.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kpushbutton.h>

DebuggerManager::DebuggerManager(MainWindow* window, QObject *parent, const char* name)
    : QObject(parent, name), m_debugger(0), m_window(window)
{
}

void DebuggerManager::init()
{
  //-----WATCH UI
  connect(m_window->btAddWatch(), SIGNAL(clicked()),
          this, SLOT(slotAddWatch()));
  connect(m_window->edAddWatch(), SIGNAL(returnPressed()),
          this, SLOT(slotAddWatch()));

  connect(m_window->watchList(), SIGNAL(sigWatchRemoved(Variable*)),
          this, SLOT(slotWatchRemoved(Variable*)));

  /*
  connect(m_window->watchList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotVarModified(Variable*)));
  */

  //-------STACK UI
  connect(m_window->stackCombo(),
          SIGNAL(changed(DebuggerExecutionLine*, DebuggerExecutionLine*)), this,
          SLOT(slotComboStackChanged(DebuggerExecutionLine*, DebuggerExecutionLine*)));

  //-----VARS UI
  connect(m_window->globalVarList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotVarModified(Variable*)));

  connect(m_window->localVarList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotVarModified(Variable*)));

  //---BREAKPOINT UI -- TextEditor * BreakpointListview
  connect(m_window->tabEditor(),
          SIGNAL(sigBreakpointMarked(QString, int )),
          m_window->breakpointListView(),
          SLOT(slotBreakpointMarked(QString, int)));

  connect(m_window->tabEditor(),
          SIGNAL(sigBreakpointUnmarked(QString, int )),
          m_window->breakpointListView(),
          SLOT(slotBreakpointUnmarked(QString, int)));

  //------BREAKPOINT UI (from listview)
  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointCreated(DebuggerBreakpoint*)));

  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointChanged(DebuggerBreakpoint*)));

  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointRemoved(DebuggerBreakpoint*)));

  /*
  connect(m_window->breakpointListView(), SIGNAL(sigBrekpointDeleted(DebuggerBreakpoint*)),
          this, SLOT(slotBrekpointDeleted(DebuggerBreakpoint*)));
  */

  disableAllDebugActions();

  slotConfigurationChanged();
  loadDebugger();
}

void DebuggerManager::disableAllDebugActions()
{
  m_window->actionCollection()->action("debug_start_session")->setEnabled(false);
  m_window->actionCollection()->action("debug_end_session")->setEnabled(false);
  m_window->actionCollection()->action("debug_run")->setEnabled(false);
  m_window->actionCollection()->action("debug_stop")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_into")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_over")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_out")->setEnabled(false);
}

DebuggerManager::~DebuggerManager()
{
  clearDebugger();
}

void DebuggerManager::clearDebugger()
{
  if(m_debugger) m_debugger->endSession();

  disableAllDebugActions();

  delete m_debugger;
  m_debugger = NULL;
}

void DebuggerManager::slotConfigurationChanged()
{
  if(m_debugger) {
    m_debugger->loadConfiguration();
  }
}

void DebuggerManager::loadDebugger()
{
  if(DebuggerSettings::client() == -1) {
    clearDebugger();
    return;
  }

  if(m_debugger && (m_debugger->id() == DebuggerSettings::client())) {
    //is the same current debugger, just load its settings again
    m_debugger->endSession();
    m_debugger->loadConfiguration();
  } else {
    if(m_debugger) clearDebugger();

    if((m_debugger = DebuggerFactory::buildDebugger(this)) == NULL) {
      m_window->showError("Error loading debugger client");
      disableAllDebugActions();
      return;
    }

    m_debugger->loadConfiguration();
    connectDebugger();
  }

  m_window->actionCollection()->action("debug_start_session")->setEnabled(true);
}

void DebuggerManager::connectDebugger()
{

  connect(m_debugger, SIGNAL(sigSessionStarted()),
          this, SLOT(slotSessionStarted()));

  connect(m_debugger, SIGNAL(sigSessionEnded()),
          this, SLOT(slotSessionEnded()));

  connect(m_debugger, SIGNAL(sigDebugStarted()),
          this, SLOT(slotDebugStarted()));

  connect(m_debugger, SIGNAL(sigDebugEnded()),
          this, SLOT(slotDebugEnded()));

  connect(m_debugger, SIGNAL(sigVariablesChanged(VariablesList_t*, bool)),
          this, SLOT(slotVariablesChanged(VariablesList_t*, bool)));

  connect(m_debugger, SIGNAL(sigStackChanged(DebuggerStack*)),
          this, SLOT(slotStackChanged(DebuggerStack*)));

  connect(m_debugger, SIGNAL(sigDebugMessage(int, QString, int, QString)),
          this, SLOT(slotDebugMessage(int, QString, int, QString)));

  connect(m_debugger, SIGNAL(sigDebugOutput(QString)),
          this, SLOT(slotDebugOutput(QString)));

  connect(m_debugger, SIGNAL(sigDebugError(QString)),
          this, SLOT(slotDebugError(QString)));

  connect(m_debugger, SIGNAL(sigInternalError(QString)),
          this, SLOT(slotInternalError(QString)));

  connect(m_debugger, SIGNAL(sigWatchChanged(Variable*)),
          this, SLOT(slotWatchChanged(Variable*)));

  connect(m_debugger, SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
          this, SLOT(slotDebugBreakpointChanged(DebuggerBreakpoint*)));
}

void DebuggerManager::slotVariablesChanged(VariablesList_t* list, bool isglobalContext)
{
  if(isglobalContext) {
    m_window->globalVarList()->setVariables(list);
  } else {
    m_window->localVarList()->setVariables(list);
  }
}

void DebuggerManager::slotWatchChanged(Variable* var)
{
  m_window->watchList()->addWatch(var);
}

void DebuggerManager::slotStackChanged(DebuggerStack* stack)
{
  //**** dealing with the current stackCombo (MARKS stuff)
  //-if stackCombo isn't empty, unmark the previously marked ExecutionMark
  // (wich is the top context of the comboStack)
  //-if stackCombo is not pointed to the top context, unmark the previously
  // marked PreExecutionLine and request the vars for this context

  DebuggerExecutionLine* execLine;
  EditorTabWidget* ed = m_window->tabEditor();


  if(m_window->stackCombo()->count() > 0) {
    execLine =
      m_window->stackCombo()->stack()->topExecutionLine();

    ed->unmarkExecutionLine(execLine->filePath(), execLine->line());
  }

  if(m_window->stackCombo()->currentItem() != 0) {
    execLine =
      m_window->stackCombo()->selectedDebuggerExecutionLine();

    ed->unmarkPreExecutionLine(execLine->filePath(), execLine->line());
  }

  //**** dealing with the new Stack (the argument)
  //-set the current document/line according to the top context of the stack
  //-mark the new ExecutionLine
  //-sets the stack to the comboStack

  execLine = stack->topExecutionLine();
  ed->setCurrentDocument(execLine->filePath(), true);
  ed->gotoLineAtFile(execLine->filePath(), execLine->line()-1);

  ed->markExecutionLine(execLine->filePath(), execLine->line());

  m_window->stackCombo()->setStack(stack);

  //----updating data relative to the current stack context

  //--request the local vars

  execLine =
    m_window->stackCombo()->selectedDebuggerExecutionLine();

  if(m_debugger) {
    m_debugger->requestLocalVariables(execLine);
    //---requesting watches
    m_debugger->requestWatches(execLine);
  }
}

void DebuggerManager::slotDebugBreakpointChanged(DebuggerBreakpoint* bp)
{
  m_window->breakpointListView()->updateBreakpoint(bp);
}

/*** Application - DebuggerManager
 */


void DebuggerManager::slotDebugStartSession()
{
  if(m_debugger) {
    m_debugger->startSession();
  } else {
    m_window->showError("No debugger selected");
  }
}

void DebuggerManager::slotDebugEndSession()
{
  if(!m_debugger) return;

  m_debugger->endSession();
}

void DebuggerManager::slotDebugRun()
{
  if(!m_debugger) return;

  if(m_window->tabEditor()->count() != 0) {
    QString filepath = m_window->tabEditor()->documentPath(
                         m_window->tabEditor()->currentPageIndex());
    m_debugger->run(filepath);
  } else {
    m_window->showSorry("Open a file to debug");
  }
}

void DebuggerManager::slotDebugStop()
{
  if(!m_debugger) return;
  m_debugger->stop();
}

void DebuggerManager::slotDebugStepInto()
{
  if(!m_debugger) return;
  m_debugger->stepInto();
}

void DebuggerManager::slotDebugStepOver()
{
  if(!m_debugger) return;
  m_debugger->stepOver();
}

void DebuggerManager::slotDebugStepOut()
{
  if(!m_debugger) return;
  m_debugger->stepOut();
}

void DebuggerManager::slotDebugToggleBp()
{
  if(!m_debugger) return;
  m_debugger->stepOut();

}

void DebuggerManager::slotAddWatch()
{
  QString expression = m_window->edAddWatch()->text();
  m_window->edAddWatch()->clear();

  if(!expression.isEmpty()) {
    if(m_debugger) {
      m_debugger->addWatch(expression);
    }
  }
}

void DebuggerManager::slotComboStackChanged(DebuggerExecutionLine* old, DebuggerExecutionLine* nw)
{
  //-set the current document/line according to the new stack context
  //-unmark the (possibly) previously PreExecutionLine according to the old stack context
  //-mark the PreExecutionLine of the new stack context
  //-request the variables for this context

  EditorTabWidget* ed = m_window->tabEditor();

  m_window->tabEditor()->setCurrentDocument(nw->filePath(), true);
  ed->gotoLineAtFile(nw->filePath(), nw->line()-1);

  ed->unmarkPreExecutionLine(old->filePath(), old->line());

  if(nw != m_window->stackCombo()->stack()->topExecutionLine()) {
    ed->markPreExecutionLine(nw->filePath(), nw->line());
  }

  if(m_debugger) {
    m_debugger->requestLocalVariables(nw);
  }
}

void DebuggerManager::slotVarModified(Variable* var)
{
  if(!m_debugger) return;

  m_debugger->modifyVariable(var,
                             m_window->stackCombo()->selectedDebuggerExecutionLine());
}

/*
void DebuggerManager::slotBreakpointMarked(QString filePath, int line) {
  m_debugger->addBreakpoint(filePath, line);
  //m_window->breakpointListView()->addBreakpoint(filePath, line);
}

void DebuggerManager::slotBreakpointUnmark(QString filePath, int line) {
  //m_debugger->removeBreakpoint(filePath, line);
  //m_window->breakpointListView()->removeBreakpoint(filePath, line);
}
*/

void DebuggerManager::slotBreakpointCreated(DebuggerBreakpoint* bp)
{
  if(!m_debugger) return;
  m_debugger->addBreakpoint(bp);
}

void DebuggerManager::slotBreakpointChanged(DebuggerBreakpoint* bp)
{
  if(!m_debugger) return;
  m_debugger->changeBreakpoint(bp);
}

void DebuggerManager::slotBreakpointRemoved(DebuggerBreakpoint* bp)
{
  m_window->tabEditor()->unmarkActiveBreakpoint(
    bp->filePath(), bp->line());

  if(m_debugger) {
    m_debugger->removeBreakpoint(bp);
  }
}

/*
void DebuggerManager::slotBreakpointDeleted(DebuggerBreakpoint* bp)
{
  m_window->tabEditor()->unmarkActiveBreakpoint(
    bp->filePath(), bp->line());

  if(m_debugger)
  {
    m_debugger->removeBreakpoint(bp);
  }
}
*/

void DebuggerManager::slotWatchRemoved(Variable* var)
{
  if(!m_debugger) return;

  m_debugger->removeWatch(var->name());
}

/*** DebuggerClient - DebuggerManager
 */

void DebuggerManager::slotSessionStarted()
{
  m_window->actionCollection()->action("debug_start_session")->setEnabled(false);
  m_window->actionCollection()->action("debug_end_session")->setEnabled(true);
  m_window->actionCollection()->action("debug_run")->setEnabled(true);
}

void DebuggerManager::slotSessionEnded()
{
  disableAllDebugActions();
  m_window->actionCollection()->action("debug_start_session")->setEnabled(true);
}

void DebuggerManager::slotDebugStarted()
{
  m_debugger->addBreakpoints(
    m_window->breakpointListView()->breakpoints());

  m_window->globalVarList()->clear();
  m_window->localVarList()->clear();
  m_window->logListView()->clear();
  m_window->stackCombo()->clear();

  m_window->statusBar()->message("Debug started");

  m_window->actionCollection()->action("debug_stop")->setEnabled(true);
  m_window->actionCollection()->action("debug_step_into")->setEnabled(true);
  m_window->actionCollection()->action("debug_step_over")->setEnabled(true);
  m_window->actionCollection()->action("debug_step_out")->setEnabled(true);
}

void DebuggerManager::slotDebugEnded()
{
  m_window->breakpointListView()->resetBreakpointItems();

  m_window->statusBar()->message("Debug ended");


  EditorTabWidget* ed = m_window->tabEditor();

  DebuggerStack* stack = m_window->stackCombo()->stack();

  if(stack) {
    //remove the execution line mark
    DebuggerExecutionLine* execLine;
    execLine = stack->topExecutionLine();

    ed->unmarkExecutionLine(execLine->filePath(), execLine->line());

    //remove the pre execution line mark if any
    execLine =
      m_window->stackCombo()->selectedDebuggerExecutionLine();

    ed->unmarkPreExecutionLine(execLine->filePath(), execLine->line());
  }

  m_window->actionCollection()->action("debug_stop")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_into")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_over")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_out")->setEnabled(false);

}

void DebuggerManager::slotDebugError(QString message)
{
  m_window->showError(message);
}

void DebuggerManager::slotInternalError(QString message)
{
  m_window->showError(message);
}


void DebuggerManager::slotDebugMessage(int type, QString message,
                                       int line, QString file)
{

  m_window->logListView()->add(type, message, line, file);
}

void DebuggerManager::slotDebugOutput(QString str)
{
  m_window->edOutput()->setText(str);
}

#include "debuggermanager.moc"
