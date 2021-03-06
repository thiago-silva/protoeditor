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
#include "settings.h"

#include <kapplication.h>
#include <kcombobox.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <klineedit.h>
//#include <ktexteditor/editinterface.h>
#include <ktextedit.h>
#include <kpushbutton.h>

DebuggerManager::DebuggerManager(MainWindow* window, QObject *parent, const char* name)
    : QObject(parent, name), m_debugger(0), m_window(window)
{
}

/******************************* internal functions ******************************************/

void DebuggerManager::init()
{
  //-----WATCH UI
  connect(m_window->btAddWatch(), SIGNAL(clicked()),
          this, SLOT(slotAddWatch()));
  connect(m_window->edAddWatch(), SIGNAL(returnPressed()),
          this, SLOT(slotAddWatch()));

  connect(m_window->watchList(), SIGNAL(sigWatchRemoved(Variable*)),
          this, SLOT(slotWatchRemoved(Variable*)));


  connect(m_window->watchList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotVarModified(Variable*)));


  //-------STACK UI
  connect(m_window->stackCombo(),
          SIGNAL(changed(DebuggerExecutionPoint*, DebuggerExecutionPoint*)), this,
          SLOT(slotComboStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)));

  //-----VARS UI
  connect(m_window->globalVarList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotVarModified(Variable*)));

  connect(m_window->localVarList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotVarModified(Variable*)));

  //---BREAKPOINT UI -- TextEditor * BreakpointListview
  connect(m_window->tabEditor(),
          SIGNAL(sigBreakpointMarked(const QString&, int )),
          m_window->breakpointListView(),
          SLOT(slotBreakpointMarked(const QString&, int)));

  connect(m_window->tabEditor(),
          SIGNAL(sigBreakpointUnmarked(const QString&, int )),
          m_window->breakpointListView(),
          SLOT(slotBreakpointUnmarked(const QString&, int)));

  connect(m_window->tabEditor(), SIGNAL(sigNewDocument()),
    this, SLOT(slotNewDocument()));

  //------BREAKPOINT UI (from listview)
  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointCreated(DebuggerBreakpoint*)));

  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointChanged(DebuggerBreakpoint*)));

  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointRemoved(DebuggerBreakpoint*)));

  connect(m_window->breakpointListView(), SIGNAL(sigDoubleClick(const QString&, int)),
    this, SLOT(slotGotoLineAtFile(const QString&, int)));

  //-------- LOG
  connect(m_window->logListView(), SIGNAL(sigDoubleClick(const QString&, int)),
    this, SLOT(slotGotoLineAtFile(const QString&, int)));

  /*
  connect(m_window->breakpointListView(), SIGNAL(sigBrekpointDeleted(DebuggerBreakpoint*)),
          this, SLOT(slotBrekpointDeleted(DebuggerBreakpoint*)));
  */

  disableAllDebugActions();
  slotConfigurationChanged();
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


void DebuggerManager::loadDebugger()
{
  if(Settings::client() == -1) {
    clearDebugger();
    return;
  }

  if(m_debugger && (m_debugger->id() == Settings::client())) {
    //is the same current debugger, just load its settings again
    m_debugger->reloadConfiguration();
  } else {
    if(m_debugger) clearDebugger();

    if((m_debugger = DebuggerFactory::buildDebugger(this)) == NULL) {
      m_window->showError("Error loading debugger client");
      disableAllDebugActions();
      return;
    }

    m_debugger->reloadConfiguration();
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


  connect(m_debugger, SIGNAL(sigInternalError(const QString&)),
          this, SLOT(slotInternalError(const QString&)));
}

/******************************* Application interface ******************************************/

void DebuggerManager::slotConfigurationChanged()
{
  loadDebugger();
  //if(m_debugger) {
  //  m_debugger->reloadConfiguration();
  //}
}

void DebuggerManager::slotDebugStartSession()
{
  if(m_debugger) {
    m_debugger->startSession();
  } else {
    m_window->showSorry("No debugger selected");
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
    QString filepath = m_window->tabEditor()->currentDocumentPath();
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
  QString filePath = m_window->tabEditor()->currentDocumentPath();
  int line = m_window->tabEditor()->currentDocumentLine();

  if(!m_window->tabEditor()->hasBreakpointAt(filePath, line)) {
    m_window->tabEditor()->markActiveBreakpoint(filePath, line);
  } else {
    m_window->tabEditor()->unmarkActiveBreakpoint(filePath, line);
  }

  m_window->breakpointListView()->toggleBreakpoint(filePath, line);
}

void DebuggerManager::slotAddWatch()
{
  QString expression = m_window->edAddWatch()->text();
  m_window->edAddWatch()->clear();

  if(!expression.isEmpty()) {
    if(m_debugger) {
      m_debugger->addWatch(expression,
        m_window->stackCombo()->stack()->topExecutionPoint());
    }
  }
}

void DebuggerManager::slotComboStackChanged(DebuggerExecutionPoint* old, DebuggerExecutionPoint* nw)
{
  //-set the current document/line according to the new stack context
  //-unmark the (possibly) previously PreExecutionPoint according to the old stack context
  //-mark the PreExecutionPoint of the new stack context
  //-request the variables for this context

  EditorTabWidget* ed = m_window->tabEditor();

  //m_window->tabEditor()->setCurrentDocument(nw->filePath(), true);
  ed->gotoLineAtFile(nw->filePath(), nw->line()-1);

  ed->unmarkPreExecutionPoint(old->filePath(), old->line());

  if(nw != m_window->stackCombo()->stack()->topExecutionPoint()) {
    ed->markPreExecutionPoint(nw->filePath(), nw->line());
  }

  if(m_debugger) {
    m_debugger->requestLocalVariables(nw);
  }
}

void DebuggerManager::slotVarModified(Variable* var)
{
  if(!m_debugger) return;

  m_debugger->modifyVariable(var,
                             m_window->stackCombo()->selectedDebuggerExecutionPoint());
}

void DebuggerManager::slotBreakpointCreated(DebuggerBreakpoint* bp)
{
  if(!m_debugger) return;
  m_debugger->addBreakpoint(bp);
}

void DebuggerManager::slotBreakpointChanged(DebuggerBreakpoint* bp)
{
  if(m_debugger) {
    m_debugger->changeBreakpoint(bp);
  }

  switch(bp->status()) {
    case DebuggerBreakpoint::ENABLED:
      m_window->tabEditor()->unmarkDisabledBreakpoint(bp->filePath(), bp->line());
      m_window->tabEditor()->markActiveBreakpoint(bp->filePath(), bp->line());
      break;
    case DebuggerBreakpoint::DISABLED:
      m_window->tabEditor()->unmarkActiveBreakpoint(bp->filePath(), bp->line());
      m_window->tabEditor()->markDisabledBreakpoint(bp->filePath(), bp->line());
    case DebuggerBreakpoint::UNRESOLVED:
      break;
  }
}

void DebuggerManager::slotBreakpointRemoved(DebuggerBreakpoint* bp)
{
  m_window->tabEditor()->unmarkActiveBreakpoint(
    bp->filePath(), bp->line());

  if(m_debugger) {
    m_debugger->removeBreakpoint(bp);
  }
}

void DebuggerManager::slotGotoLineAtFile(const QString& filePath, int line)
{
  m_window->tabEditor()->gotoLineAtFile(filePath, line-1);
}

void DebuggerManager::slotWatchRemoved(Variable* var)
{
  if(!m_debugger) return;

  m_debugger->removeWatch(var->name());
}

void DebuggerManager::slotNewDocument()
{
  //if the new document has breakpoints, mark them.

  QValueList<DebuggerBreakpoint*> bplist =
    m_window->breakpointListView()->breakpointsFrom(
      m_window->tabEditor()->currentDocumentPath());

  QValueList<DebuggerBreakpoint*>::iterator it;
  for(it = bplist.begin(); it != bplist.end(); ++it) {
    m_window->tabEditor()->markActiveBreakpoint((*it)->filePath(), (*it)->line());
  }
}

/******************************* Debugger interface ******************************************/

void DebuggerManager::updateStack(DebuggerStack* stack)
{
  //**** dealing with the current stackCombo (MARKS stuff)
  //-if stackCombo isn't empty, unmark the previously marked ExecutionMark
  // (wich is the top context of the comboStack)
  //-if stackCombo is not pointed to the top context, unmark the previously
  // marked PreExecutionPoint and request the vars for this context

  //Conveniece vars:
  DebuggerExecutionPoint* execPoint;
  EditorTabWidget* ed = m_window->tabEditor();


  if(m_window->stackCombo()->count() > 0) {
    execPoint =
      m_window->stackCombo()->stack()->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->filePath(), execPoint->line());
  }

  if(m_window->stackCombo()->currentItem() != 0) {
    execPoint =
      m_window->stackCombo()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->filePath(), execPoint->line());
  }

  //**** dealing with the new Stack (the argument)
  //-set the current document/line according to the top context of the stack
  //-mark the new ExecutionPoint
  //-sets the stack to the comboStack

  //lets update first and force the user to be on the top of the stack
  m_window->stackCombo()->setStack(stack);

  execPoint = stack->topExecutionPoint();
  ed->setCurrentDocument(execPoint->filePath(), true);
  ed->gotoLineAtFile(execPoint->filePath(), execPoint->line()-1);

  ed->markExecutionPoint(execPoint->filePath(), execPoint->line());

  //--request stack context dependent vars

  //if(m_debugger) {
  //  m_debugger->requestLocalVariables(execPoint);
  //  //---requesting watches
  //  m_debugger->requestWatches(execPoint);
  //}
}

void DebuggerManager::updateGlobalVars(VariablesList_t* vars) {
  m_window->globalVarList()->setVariables(vars);
}

void DebuggerManager::updateLocalVars(VariablesList_t* vars)
{
  m_window->localVarList()->setVariables(vars);
}

void DebuggerManager::updateWatch(Variable* var)
{
  m_window->watchList()->addWatch(var);
}

void DebuggerManager::updateBreakpoint(DebuggerBreakpoint* bp)
{
  m_window->breakpointListView()->updateBreakpoint(bp);
}

void DebuggerManager::debugMessage(int type, const QString& msg, const QString& filePath, int line)
{
  m_window->logListView()->add(type, msg, line, filePath);
}

void DebuggerManager::debugError(const QString& msg)
{
  m_window->showError(msg);
}

void DebuggerManager::updateOutput(const QString& output) {
  m_window->edOutput()->setText(output);
}

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
  m_window->edOutput()->clear();

  m_debugger->addBreakpoints(
    m_window->breakpointListView()->breakpoints());

  m_window->globalVarList()->setReadOnly(false);
  m_window->localVarList()->setReadOnly(false);
  m_window->watchList()->setReadOnly(false);

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

  m_window->globalVarList()->setReadOnly(true);
  m_window->localVarList()->setReadOnly(true);
  m_window->watchList()->setReadOnly(true);


  m_window->statusBar()->message("Debug ended");


  EditorTabWidget* ed = m_window->tabEditor();

  DebuggerStack* stack = m_window->stackCombo()->stack();

  if(stack) {
    //remove the execution line mark
    DebuggerExecutionPoint* execPoint;
    execPoint = stack->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->filePath(), execPoint->line());

    //remove the pre execution line mark if any
    execPoint =
      m_window->stackCombo()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->filePath(), execPoint->line());
  }

  m_window->actionCollection()->action("debug_stop")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_into")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_over")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_out")->setEnabled(false);

}

void DebuggerManager::slotInternalError(const QString& message)
{
  m_window->showError(message);
}

#include "debuggermanager.moc"
