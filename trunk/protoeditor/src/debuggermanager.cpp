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
#include "debuggerconfigurations.h"
#include "debuggerstack.h"
#include "dbugrconfigform.h"
#include "editortabwidget.h"
#include "abstractdebugger.h"
#include "debuggerfactory.h"
#include "protoeditor.h"
#include "mainwindow.h"
#include "debuggercombostack.h"
#include "variableslistview.h"
#include "watchlistview.h"
#include "loglistview.h"
#include <kapplication.h>
#include <qcombobox.h>
#include <qstatusbar.h>
#include <qaction.h>
#include <qlineedit.h>
#include <ktextedit.h>
#include <qpushbutton.h>
#include "debuggerbreakpoint.h"
#include "breakpointlistview.h"

DebuggerManager::DebuggerManager(ProtoEditor* protoEditor, QObject *parent, const char* name)
    : QObject(parent, name), m_debugger(0), m_debuggerConfigurations(0),
    m_protoEditor(protoEditor)
{
  //---------MENU
  connect(m_protoEditor->mainWindow()->debuggerStartSessionAction(), SIGNAL(activated()) , this, SLOT(slotDebuggerStartSession()));
  connect(m_protoEditor->mainWindow()->debuggerEndSessionAction(), SIGNAL(activated()) , this, SLOT(slotDebuggerEndSession()));
  connect(m_protoEditor->mainWindow()->debuggerOptionsAction(), SIGNAL(activated()) , this, SLOT(slotShowDebuggerOptions()));
  connect(m_protoEditor->mainWindow()->debuggerRunAction(), SIGNAL(activated()) , this, SLOT(slotDebuggerRun()));
  connect(m_protoEditor->mainWindow()->debuggerStopAction(), SIGNAL(activated()), this, SLOT(slotDebuggerStop()));
  connect(m_protoEditor->mainWindow()->debuggerStepIntoAction(), SIGNAL(activated()) , this, SLOT(slotDebuggerStepInto()));
  connect(m_protoEditor->mainWindow()->debuggerStepOverAction(), SIGNAL(activated()) , this, SLOT(slotDebuggerStepOver()));
  connect(m_protoEditor->mainWindow()->debuggerStepOutAction(), SIGNAL(activated()) , this, SLOT(slotDebuggerStepOut()));

  //-----WATCH UI
  connect(m_protoEditor->mainWindow()->btAddWatch(), SIGNAL(clicked()),
          this, SLOT(slotAddWatch()));
  connect(m_protoEditor->mainWindow()->edAddWatch(), SIGNAL(returnPressed()),
          this, SLOT(slotAddWatch()));

  connect(m_protoEditor->mainWindow()->watchList(), SIGNAL(sigWatchRemoved(Variable*)),
          this, SLOT(slotWatchRemoved(Variable*)));

  //-------STACK UI
  connect(m_protoEditor->mainWindow()->stackCombo(),
          SIGNAL(changed(DebuggerExecutionLine*, DebuggerExecutionLine*)), this,
          SLOT(slotComboStackChanged(DebuggerExecutionLine*, DebuggerExecutionLine*)));

  //-----VARS UI
  connect(m_protoEditor->mainWindow()->globalVarList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotVarModified(Variable*)));

  connect(m_protoEditor->mainWindow()->localVarList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotVarModified(Variable*)));

  //---BREAKPOINT UI -- TextEditor * BreakpointListview
  connect(m_protoEditor->mainWindow()->tabEditor(),
          SIGNAL(sigBreakpointMarked(QString, int )),
          m_protoEditor->mainWindow()->breakpointListView(),
          SLOT(slotBreakpointMarked(QString, int)));

  connect(m_protoEditor->mainWindow()->tabEditor(),
          SIGNAL(sigBreakpointUnmarked(QString, int )),
          m_protoEditor->mainWindow()->breakpointListView(),
          SLOT(slotBreakpointUnmarked(QString, int)));

  //------BREAKPOINT UI (from listview)
  connect(m_protoEditor->mainWindow()->breakpointListView(), SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointCreated(DebuggerBreakpoint*)));

  connect(m_protoEditor->mainWindow()->breakpointListView(), SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointChanged(DebuggerBreakpoint*)));

  connect(m_protoEditor->mainWindow()->breakpointListView(), SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointRemoved(DebuggerBreakpoint*)));

  /*
  connect(m_protoEditor->mainWindow()->breakpointListView(), SIGNAL(sigBrekpointDeleted(DebuggerBreakpoint*)),
          this, SLOT(slotBrekpointDeleted(DebuggerBreakpoint*)));
  */

  m_protoEditor->mainWindow()->debuggerEndSessionAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerOptionsAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerRunAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStartSessionAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStepIntoAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStepOutAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStepOverAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStopAction()->setEnabled(false);

  m_debuggerConfigurations = new DebuggerConfigurations();

  m_debuggerConfigurations->readConfigurations(
    KApplication::kApplication()->config());
  if(m_debuggerConfigurations->debuggerId() != -1)
  {
    loadDebugger();
  }
  m_protoEditor->mainWindow()->debuggerOptionsAction()->setEnabled(true);
}

DebuggerManager::~DebuggerManager()
{
  delete m_debuggerConfigurations;
  clearDebugger();
}

void DebuggerManager::clearDebugger()
{
  delete m_debugger;
  m_debugger = NULL;
}

void DebuggerManager::loadDebugger()
{
  if(m_debugger)
  {
    m_debugger->endSession();
  }

  if(!m_debugger)
  {
    m_debugger = DebuggerFactory::buildDebugger(this, m_debuggerConfigurations);
    if(m_debugger) connectDebugger();
  }
  else if(m_debuggerConfigurations->debuggerId() != m_debugger->id())
  {
    clearDebugger();
    m_debugger = DebuggerFactory::buildDebugger(this, m_debuggerConfigurations);
    if(m_debugger) connectDebugger();
  }
  else
  {
    m_debugger->loadConfiguration(m_debuggerConfigurations);
  }

  if(!m_debugger)
    return;

  m_protoEditor->mainWindow()->debuggerStartSessionAction()->setEnabled(true);
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
  if(isglobalContext)
  {
    m_protoEditor->mainWindow()->globalVarList()->setVariables(list);
  }
  else
  {
    m_protoEditor->mainWindow()->localVarList()->setVariables(list);
  }
}

void DebuggerManager::slotWatchChanged(Variable* var)
{
  m_protoEditor->mainWindow()->watchList()->addWatch(var);
}

void DebuggerManager::slotStackChanged(DebuggerStack* stack)
{
  //**** dealing with the current stackCombo (MARKS stuff)
  //-if stackCombo isn't empty, unmark the previously marked ExecutionMark
  // (wich is the top context of the comboStack)
  //-if stackCombo is not pointed to the top context, unmark the previously
  // marked PreExecutionLine and request the vars for this context

  DebuggerExecutionLine* execLine;
  EditorTabWidget* ed = m_protoEditor->mainWindow()->tabEditor();


  if(m_protoEditor->mainWindow()->stackCombo()->count() > 0)
  {
    execLine =
      m_protoEditor->mainWindow()->stackCombo()->stack()->topExecutionLine();

    ed->unmarkExecutionLine(execLine->filePath(), execLine->line());
  }

  if(m_protoEditor->mainWindow()->stackCombo()->currentItem() != 0)
  {
    execLine =
      m_protoEditor->mainWindow()->stackCombo()->selectedDebuggerExecutionLine();

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

  m_protoEditor->mainWindow()->stackCombo()->setStack(stack);

  //----updating data relative to the current stack context

  //--request the local vars

  execLine =
    m_protoEditor->mainWindow()->stackCombo()->selectedDebuggerExecutionLine();

  if(m_debugger)
  {
    m_debugger->requestLocalVariables(execLine);
    //---requesting watches
    m_debugger->requestWatches(execLine);
  }
}

void DebuggerManager::slotDebugBreakpointChanged(DebuggerBreakpoint* bp)
{
  m_protoEditor->mainWindow()->breakpointListView()->updateBreakpoint(bp);
}

/*** Application - DebuggerManager
 */
void DebuggerManager::slotShowDebuggerOptions()
{
  DebuggerConfigForm* configForm = new DebuggerConfigForm(
                                     m_protoEditor->mainWindow());

  configForm->setConfigurations(m_debuggerConfigurations);

  configForm->setModal(true);
  configForm->exec();

  if(configForm->optionsChanged())
  {
    delete m_debuggerConfigurations;
    m_debuggerConfigurations = configForm->configurations();
    m_debuggerConfigurations->saveConfigurations(
      KApplication::kApplication()->config());
    loadDebugger();
  }
  delete configForm;
}

void DebuggerManager::slotDebuggerStartSession()
{
  if(m_debugger)
  {
    m_debugger->startSession();
  }
  else
  {
    m_protoEditor->mainWindow()->showError("No debugger selected");
  }
}

void DebuggerManager::slotDebuggerEndSession()
{
  if(!m_debugger) return;

  m_debugger->endSession();
}

void DebuggerManager::slotDebuggerRun()
{
  if(!m_debugger) return;

  if(m_protoEditor->mainWindow()->tabEditor()->count() != 0) {
    QString filepath = m_protoEditor->mainWindow()->tabEditor()->documentPath(
                       m_protoEditor->mainWindow()->tabEditor()->currentPageIndex());
    m_debugger->run(filepath);
  } else {
    m_protoEditor->mainWindow()->showError("Select a file first");
  }
}

void DebuggerManager::slotDebuggerStop()
{
  if(!m_debugger) return;
  m_debugger->stop();
}

void DebuggerManager::slotDebuggerStepInto()
{
  if(!m_debugger) return;
  m_debugger->stepInto();
}

void DebuggerManager::slotDebuggerStepOver()
{
  if(!m_debugger) return;
  m_debugger->stepOver();
}

void DebuggerManager::slotDebuggerStepOut()
{
  if(!m_debugger) return;
  m_debugger->stepOut();
}

void DebuggerManager::slotAddWatch()
{
  QString expression = m_protoEditor->mainWindow()->edAddWatch()->text();
  m_protoEditor->mainWindow()->edAddWatch()->clear();

  if(!expression.isEmpty())
  {
    if(m_debugger)
    {
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

  EditorTabWidget* ed = m_protoEditor->mainWindow()->tabEditor();

  m_protoEditor->mainWindow()->tabEditor()->setCurrentDocument(nw->filePath(), true);
  ed->gotoLineAtFile(nw->filePath(), nw->line()-1);

  ed->unmarkPreExecutionLine(old->filePath(), old->line());

  if(nw != m_protoEditor->mainWindow()->stackCombo()->stack()->topExecutionLine())
  {
    ed->markPreExecutionLine(nw->filePath(), nw->line());
  }

  if(m_debugger)
  {
    m_debugger->requestLocalVariables(nw);
  }
}

void DebuggerManager::slotVarModified(Variable* var)
{
  if(!m_debugger) return;

  m_debugger->modifyVariable(var,
                             m_protoEditor->mainWindow()->stackCombo()->selectedDebuggerExecutionLine());
}

/*
void DebuggerManager::slotBreakpointMarked(QString filePath, int line) {
  m_debugger->addBreakpoint(filePath, line);
  //m_protoEditor->mainWindow()->breakpointListView()->addBreakpoint(filePath, line);
}

void DebuggerManager::slotBreakpointUnmark(QString filePath, int line) {
  //m_debugger->removeBreakpoint(filePath, line);
  //m_protoEditor->mainWindow()->breakpointListView()->removeBreakpoint(filePath, line);
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
  m_protoEditor->mainWindow()->tabEditor()->unmarkActiveBreakpoint(
    bp->filePath(), bp->line());

  if(m_debugger)
  {
    m_debugger->removeBreakpoint(bp);
  }
}

/*
void DebuggerManager::slotBreakpointDeleted(DebuggerBreakpoint* bp)
{
  m_protoEditor->mainWindow()->tabEditor()->unmarkActiveBreakpoint(
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
  m_protoEditor->mainWindow()->debuggerEndSessionAction()->setEnabled(true);
  m_protoEditor->mainWindow()->debuggerStartSessionAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerRunAction()->setEnabled(true);
}

void DebuggerManager::slotSessionEnded()
{
  m_protoEditor->mainWindow()->debuggerEndSessionAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStartSessionAction()->setEnabled(true);
  m_protoEditor->mainWindow()->debuggerRunAction()->setEnabled(false);
}

void DebuggerManager::slotDebugStarted()
{
  m_debugger->addBreakpoints(
    m_protoEditor->mainWindow()->breakpointListView()->breakpoints());


  m_protoEditor->mainWindow()->statusBar()->message("Debug started");

  m_protoEditor->mainWindow()->debuggerStepIntoAction()->setEnabled(true);
  m_protoEditor->mainWindow()->debuggerStepOutAction()->setEnabled(true);
  m_protoEditor->mainWindow()->debuggerStepOverAction()->setEnabled(true);
  m_protoEditor->mainWindow()->debuggerStopAction()->setEnabled(true);
}

void DebuggerManager::slotDebugEnded()
{
  m_protoEditor->mainWindow()->breakpointListView()->resetBreakpointItems();

  m_protoEditor->mainWindow()->statusBar()->message("Debug ended");

  //m_protoEditor->mainWindow()->varList()->clear();

  //m_protoEditor->mainWindow()->logListView()->clear();

  EditorTabWidget* ed = m_protoEditor->mainWindow()->tabEditor();

  DebuggerStack* stack = m_protoEditor->mainWindow()->stackCombo()->stack();

  if(stack)
  {
    //remove the execution line mark
    DebuggerExecutionLine* execLine;
    execLine = stack->topExecutionLine();

    ed->unmarkExecutionLine(execLine->filePath(), execLine->line());

    //remove the pre execution line mark if any
    execLine =
      m_protoEditor->mainWindow()->stackCombo()->selectedDebuggerExecutionLine();

    ed->unmarkPreExecutionLine(execLine->filePath(), execLine->line());
  }

  //m_protoEditor->mainWindow()->stackCombo()->clear();

  m_protoEditor->mainWindow()->debuggerStepIntoAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStepOutAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStepOverAction()->setEnabled(false);
  m_protoEditor->mainWindow()->debuggerStopAction()->setEnabled(false);
}

void DebuggerManager::slotDebugError(QString message)
{
  m_protoEditor->showError(message);
}

void DebuggerManager::slotInternalError(QString message)
{
  m_protoEditor->showError(message);
}


void DebuggerManager::slotDebugMessage(int type, QString message,
                                       int line, QString file)
{

  m_protoEditor->mainWindow()->logListView()->add(type, message, line, file);
}

void DebuggerManager::slotDebugOutput(QString str)
{
  m_protoEditor->mainWindow()->edOutput()->setText(str);
}

#include "debuggermanager.moc"
