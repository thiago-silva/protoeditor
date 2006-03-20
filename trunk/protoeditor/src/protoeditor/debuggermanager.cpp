/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
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
#include "messagelistview.h"
#include "debuggerbreakpoint.h"
#include "breakpointlistview.h"

#include "protoeditorsettings.h"
#include "sitesettings.h"
#include "phpsettings.h"

#include <kapplication.h>
#include <kcombobox.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <klineedit.h>
//#include <ktexteditor/editinterface.h>
#include <ktextedit.h>
#include <kpushbutton.h>
#include <kdialogbase.h>
#include <kdebug.h>

#include "session.h"

DebuggerManager::DebuggerManager(MainWindow* window, QObject *parent, const char* name)
    : QObject(parent, name), m_activeDebugger(0), m_window(window)/*, m_showProfileDialog(false)*/
{
  connect(Session::self(), SIGNAL(sigError(const QString&)),
      this, SLOT(slotError(const QString&)));
}

/******************************* internal functions ******************************************/

void DebuggerManager::init()
{
  //connects the WATCH UI to us
  connect(m_window->btAddWatch(), SIGNAL(clicked()),
          this, SLOT(slotAddWatch()));

  connect(m_window->edAddWatch(), SIGNAL(returnPressed()),
          this, SLOT(slotAddWatch()));

  connect(m_window->tabEditor(), SIGNAL(sigAddWatch(const QString&)),
          this, SLOT(slotAddWatch(const QString&)));

  connect(m_window->watchList(), SIGNAL(sigWatchRemoved(Variable*)),
          this, SLOT(slotWatchRemoved(Variable*)));

  connect(m_window->watchList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotLocalVarModified(Variable*)));


  //connects the STACK UI to us
  connect(m_window->stackCombo(),
          SIGNAL(changed(DebuggerExecutionPoint*, DebuggerExecutionPoint*)), this,
          SLOT(slotComboStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)));

  //connects the Variables UI (VariablesList local/global) to us
  connect(m_window->globalVarList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotGlobalVarModified(Variable*)));

  connect(m_window->localVarList(), SIGNAL(sigVarModified(Variable*)),
          this, SLOT(slotLocalVarModified(Variable*)));

  //connects the editor with the BREAKPOINT UI
  connect(m_window->tabEditor(),
          SIGNAL(sigBreakpointMarked(const QString&, int, bool)),
          m_window->breakpointListView(),
          SLOT(slotBreakpointMarked(const QString&, int, bool)));

  connect(m_window->tabEditor(),
          SIGNAL(sigBreakpointUnmarked(const QString&, int )),
          m_window->breakpointListView(),
          SLOT(slotBreakpointUnmarked(const QString&, int)));

  //connects the editor to us
  connect(m_window->tabEditor(), SIGNAL(sigNewDocument()),
          this, SLOT(slotNewDocument()));

  connect(m_window->tabEditor(), SIGNAL(sigNoDocument()),
          this, SLOT(slotNoDocument()));


  //connects the BREAKPOINT UI to us
  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointCreated(DebuggerBreakpoint*)));

  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointChanged(DebuggerBreakpoint*)));

  connect(m_window->breakpointListView(), SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointRemoved(DebuggerBreakpoint*)));

  connect(m_window->breakpointListView(), SIGNAL(sigDoubleClick(const QString&, int)),
          this, SLOT(slotGotoLineAtFile(const QString&, int)));

  //connects the LOG UI to us
  connect(m_window->messageListView(), SIGNAL(sigDoubleClick(const QString&, int)),
          this, SLOT(slotGotoLineAtFile(const QString&, int)));

  loadDebuggers();
}

DebuggerManager::~DebuggerManager()
{
  clearDebuggers();
}

void DebuggerManager::clearDebuggers()
{
  for(QMap<QString, AbstractDebugger*>::iterator it = m_debuggerMap.begin();
      it != m_debuggerMap.end();
      it++)
  {
    AbstractDebugger* d = it.data();
    delete d;
  }

  m_debuggerMap.clear();
}


void DebuggerManager::loadDebuggers()
{
  clearDebuggers();

  //Load all debugger clients
  
  m_debuggerMap = DebuggerFactory::buildDebuggers(this);
  for(QMap<QString,AbstractDebugger*>::iterator it = m_debuggerMap.begin();
      it != m_debuggerMap.end();
      it++)
  {
    connectDebugger(it.data());
    it.data()->init();
  }
}

void DebuggerManager::connectDebugger(AbstractDebugger* debugger)
{
  //Connect the debugger to us
  
  connect(debugger, SIGNAL(sigDebugStarted(AbstractDebugger*)),
          this, SLOT(slotDebugStarted(AbstractDebugger*)));

  connect(debugger, SIGNAL(sigDebugEnded()),
          this, SLOT(slotDebugEnded()));

  connect(debugger, SIGNAL(sigInternalError(const QString&)),
          this, SLOT(slotError(const QString&)));

  connect(debugger, SIGNAL(sigDebugBreak()),
          this, SLOT(slotDebugBreak()));

  connect(debugger, SIGNAL(sigDebugStarting()),
          this, SLOT(slotDebugStarting()));
}


/******************************* Application interface ******************************************/

void DebuggerManager::slotConfigurationChanged()
{
  loadDebuggers();
}

void DebuggerManager::slotScriptRun()
{
  if(m_window->tabEditor()->count() == 0)
  {
    m_window->openFile();
    if(m_window->tabEditor()->count() == 0)
    {
      //couldn't open the file for some reason
      return;
    }
  }

  QString filePath = m_window->tabEditor()->currentDocumentPath();

  //Session::self()->start(ProtoeditorSettings::self()->phpSettings()->PHPCommand(), filePath);
  Session::self()->start(filePath, m_window->cbArguments()->currentText());
}

QString DebuggerManager::sessionPrologue(bool isProfiling) 
{
  //1: check if there is a debug session running
  //  -if yes, send a continueExecution()
  //2: check the current Site
  //  -if exists, extract the debugger and default file
  //  -else, active debugger is the default debugger in settings
  //3: if no active debugger so far, emit error
  //4: check if filePath (from got from site) is empty
  //  if yes, get filePath from current document / or open a fileDialog
  //  if no filePath so far, return and and we are done.
  //5: debugger->start(siteSettings, filePath);
  

  if(m_activeDebugger && m_activeDebugger->isRunning())
  {
    if(isProfiling)
    {
      m_activeDebugger->stop();
    }
    else
    {
      m_activeDebugger->continueExecution();
      m_window->setDebugStatusMsg("Continuing...");
      return QString::null;
    }
  }
  
  QString filePath;
  SiteSettings* currentSite = ProtoeditorSettings::self()->currentSiteSettings();
  if(currentSite)
  {
    m_activeDebugger = m_debuggerMap[currentSite->debuggerClient()];   
 
    if(!m_window->useCurrentScript())
    {
      filePath = currentSite->defaultFile();
    }
  }
  else
  {
    m_activeDebugger = m_debuggerMap[ProtoeditorSettings::self()->phpSettings()->defaultDebugger()];
  }
  
  
  if(!m_activeDebugger)
  {
    m_window->showSorry("Error loading debugger.");
    return QString::null;
  }

  if(filePath.isEmpty())
  {
    if(m_window->tabEditor()->count() == 0)
    {
      m_window->openFile();
      if(m_window->tabEditor()->count() == 0)
      {
        //couldn't open the file for some reason
        return QString::null;
      }
    }
    filePath = m_window->tabEditor()->currentDocumentPath();
  }
  return filePath;
}

void DebuggerManager::processSession(const QString& filePath, bool local, bool isProfiling)
{
  if(isProfiling)
  {
    m_activeDebugger->profile(filePath, m_window->cbArguments()->currentText(), local);
  }
  else
  {
    m_activeDebugger->start(filePath, m_window->cbArguments()->currentText(), local);
  }
}

void DebuggerManager::slotDebugStart()
{
  QString filePath = sessionPrologue(false);
  if(!filePath.isEmpty()) {
    processSession(filePath, ProtoeditorSettings::self()->currentSiteSettings()?false:true, false);
  }
}

void DebuggerManager::slotProfile()
{
  QString filePath = sessionPrologue(true);
  if(!filePath.isEmpty()) {
    processSession(filePath, ProtoeditorSettings::self()->currentSiteSettings()?false:true, true);
  }
}

void DebuggerManager::slotDebugStop()
{
  if(!m_activeDebugger) return;

  m_window->setDebugStatusMsg("Stoping...");
  m_activeDebugger->stop();
}

void DebuggerManager::slotDebugStepInto()
{
  if(!m_activeDebugger) return;

  m_window->setDebugStatusMsg("Steping...");
  m_activeDebugger->stepInto();
}

void DebuggerManager::slotDebugStepOver()
{
  if(!m_activeDebugger) return;

  m_window->setDebugStatusMsg("Steping...");
  m_activeDebugger->stepOver();
}

void DebuggerManager::slotDebugStepOut()
{
  if(!m_activeDebugger) return;

  m_window->setDebugStatusMsg("Steping...");
  m_activeDebugger->stepOut();
}

//from the menu action
void DebuggerManager::slotDebugToggleBp()
{
  QString filePath = m_window->tabEditor()->currentDocumentPath();
  int line = m_window->tabEditor()->currentDocumentLine();

  if(!m_window->tabEditor()->hasBreakpointAt(filePath, line))
  {
    m_window->tabEditor()->markActiveBreakpoint(filePath, line);
  }
  else
  {
    m_window->tabEditor()->unmarkActiveBreakpoint(filePath, line);
  }

  m_window->breakpointListView()->toggleBreakpoint(filePath, line);
}

void DebuggerManager::slotAddWatch()
{
  QString expression = m_window->edAddWatch()->text();
  m_window->edAddWatch()->clear();

  m_window->watchList()->addWatch(expression);

  if(m_activeDebugger) 
  {
    m_activeDebugger->addWatch(expression);
  }
}

void DebuggerManager::slotAddWatch(const QString& expression)
{
  m_window->edAddWatch()->clear();

  m_window->watchList()->addWatch(expression);

  if(m_activeDebugger) 
  {
    m_activeDebugger->addWatch(expression);
  }
}

void DebuggerManager::slotComboStackChanged(DebuggerExecutionPoint* old, DebuggerExecutionPoint* nw)
{
  //Glossary:
  //  -PreExecutionPoint: a DebuggerExecutionPoint representing a point in the stack of the backtrace.
  //                      This is used when the user selects a point on the stack ComboBox that is not the
  //                      topmost (the active line the debugger is running)
  
  
  //-set the current document/line according to the new stack context
  //-unmark the (possibly) previously PreExecutionPoint according to the old stack context
  //-mark the PreExecutionPoint of the new stack context
  //-request the variables for this context

  EditorTabWidget* ed = m_window->tabEditor();

  ed->gotoLineAtFile(nw->filePath(), nw->line()-1);

  ed->unmarkPreExecutionPoint(old->filePath()/*, old->line()*/);

  if(nw != m_window->stackCombo()->stack()->topExecutionPoint())
  {
    ed->markPreExecutionPoint(nw->filePath(), nw->line());
  }

  if(m_activeDebugger)
  {
    m_activeDebugger->changeCurrentExecutionPoint(nw);
  }
}

void DebuggerManager::slotGlobalVarModified(Variable* var)
{
  if(!m_activeDebugger) return;

  m_activeDebugger->modifyVariable(var,
                                   m_window->stackCombo()->stack()->bottomExecutionPoint());
}

void DebuggerManager::slotLocalVarModified(Variable* var)
{
  if(!m_activeDebugger) return;

  m_activeDebugger->modifyVariable(var,
                                   m_window->stackCombo()->selectedDebuggerExecutionPoint());
}

void DebuggerManager::slotBreakpointCreated(DebuggerBreakpoint* bp)
{
  if(!m_activeDebugger) return;
  m_activeDebugger->addBreakpoint(bp);
}

void DebuggerManager::slotBreakpointChanged(DebuggerBreakpoint* bp)
{
  if(m_activeDebugger) {
    m_activeDebugger->changeBreakpoint(bp);
  }

  switch(bp->status())
  {
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
  if(bp->status() == DebuggerBreakpoint::ENABLED) {
    m_window->tabEditor()->unmarkActiveBreakpoint(
      bp->filePath(), bp->line());
  } else {
    m_window->tabEditor()->unmarkDisabledBreakpoint(
      bp->filePath(), bp->line());
  }

  if(m_activeDebugger)
  {
    m_activeDebugger->removeBreakpoint(bp);
  }
}

void DebuggerManager::slotGotoLineAtFile(const QString& filePath, int line)
{
  m_window->tabEditor()->gotoLineAtFile(filePath, line-1);
}

void DebuggerManager::slotWatchRemoved(Variable* var)
{
  for(QMap<QString,AbstractDebugger*>::iterator it = m_debuggerMap.begin();
      it != m_debuggerMap.end();
      it++)
  {
    it.data()->removeWatch(var->name());
  }  
}

void DebuggerManager::slotNoDocument()
{
  if(!m_activeDebugger) return;

  m_activeDebugger->stop();

}

void DebuggerManager::slotNewDocument()
{
  //if the new document has breakpoints, mark them.

  QValueList<DebuggerBreakpoint*> bplist =
    m_window->breakpointListView()->breakpointsFrom(
      m_window->tabEditor()->currentDocumentPath());

  QValueList<DebuggerBreakpoint*>::iterator it;
  for(it = bplist.begin(); it != bplist.end(); ++it)
  {
    m_window->tabEditor()->markActiveBreakpoint((*it)->filePath(), (*it)->line());
  }
}


/******************************* Debugger interface ******************************************/

void DebuggerManager::updateStack(DebuggerStack* stack)
{
  //**** dealing with the current stackCombo (MARKS stuff)
  //-if stackCombo isn't empty, unmark the previously marked ExecutionMark
  // (wich is the top context of the comboStack)
  //-if comboStack is not pointed to the top context, unmark the previously
  // marked PreExecutionPoint and request the vars for this context

  //Conveniece vars:
  DebuggerExecutionPoint* execPoint;
  EditorTabWidget* ed = m_window->tabEditor();

  if(m_window->stackCombo()->count() > 0)
  {
    execPoint =
      m_window->stackCombo()->stack()->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->filePath()/*, execPoint->line()*/);
  }

  if(m_window->stackCombo()->currentItem() != 0)
  {
    execPoint =
      m_window->stackCombo()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->filePath()/*, execPoint->line()*/);
  }

  //**** dealing with the new Stack (the argument)
  //-set the current document/line according to the top context of the stack
  //-mark the new ExecutionPoint
  //-sets the stack to the comboStack

  //lets update first and force the user to be on the top of the stack
  m_window->stackCombo()->setStack(stack);

  execPoint = stack->topExecutionPoint();
  ed->setCurrentDocumentTab(execPoint->filePath(), true);
  ed->gotoLineAtFile(execPoint->filePath(), execPoint->line()-1);

  ed->markExecutionPoint(execPoint->filePath(), execPoint->line());
}

void DebuggerManager::updateGlobalVars(VariablesList_t* vars)
{
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
  if(type == DebuggerManager::ErrorMsg) 
  {
    m_window->tabEditor()->gotoLineAtFile(filePath, line);
    m_window->showError(msg);
  }
  m_window->messageListView()->add(type, msg, line, filePath);
}

void DebuggerManager::addOutput(const QString& output)
{
   m_window->edOutput()->append(output);
}

void DebuggerManager::slotDebugStarting()
{
  //m_window->setLedState(MainWindow::LedWait);
  //m_window->setDebugStatusMsg("Starting...");
}

void DebuggerManager::slotDebugStarted(AbstractDebugger* debugger)
{
  //let the party begins...
  
  m_activeDebugger = debugger;
  
  //let the user change the variables 
  m_window->globalVarList()->setReadOnly(false);
  m_window->localVarList()->setReadOnly(false);
  m_window->watchList()->setReadOnly(false);

  //reset all watches values
  m_window->watchList()->reset();

  //clear everyone  
  m_window->globalVarList()->clear();
  m_window->localVarList()->clear();
  m_window->messageListView()->clear();
  m_window->stackCombo()->clear();
  m_window->edOutput()->clear();

  m_window->setDebugStatusMsg("Debug started");
  m_window->setDebugStatusName(m_activeDebugger->name()); //show the name of the debugger on the StatusBar
  m_window->setLedState(MainWindow::LedOn);               //green led

  //setup the actions stuff
  m_window->actionStateChanged("debug_started");
  m_window->actionCollection()->action("debug_start")->setText("Continue");

  m_window->actionCollection()->action("site_selection")->setEnabled(false);

  //send all breakpoints to the debugger 
  m_activeDebugger->addBreakpoints(
    m_window->breakpointListView()->breakpoints());

  //send all watches to the debugger
  m_activeDebugger->addWatches(m_window->watchList()->watches());
}

void DebuggerManager::slotDebugEnded()
{
  //reset all breakpoints to their original state
  m_window->breakpointListView()->resetBreakpointItems();

  //do not let the user change variables anymore
  m_window->globalVarList()->setReadOnly(true);
  m_window->localVarList()->setReadOnly(true);
  m_window->watchList()->setReadOnly(true);  

  //setup the actions stuff
  m_window->actionCollection()->action("site_selection")->setEnabled(true);
  m_window->actionStateChanged("debug_stopped");
  m_window->actionCollection()->action("debug_start")->setText("Start Debug");
  
  m_window->setDebugStatusMsg("Stopped");
  m_window->setLedState(MainWindow::LedOff); //red led

  EditorTabWidget* ed = m_window->tabEditor();

  DebuggerStack* stack = m_window->stackCombo()->stack();

  if(stack)
  {
    //remove the execution line mark
    DebuggerExecutionPoint* execPoint;
    execPoint = stack->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->filePath()/*, execPoint->line()*/);

    //remove the pre execution line mark if any
    execPoint =
      m_window->stackCombo()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->filePath()/*, execPoint->line()*/);
  }
  
  /*
  m_window->actionCollection()->action("debug_stop")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_into")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_over")->setEnabled(false);
  m_window->actionCollection()->action("debug_step_out")->setEnabled(false);
  */

  m_activeDebugger = 0;
  m_window->setDebugStatusName("");
}

void DebuggerManager::slotDebugBreak()
{
  //step(into/over/out) or a breakpoint or something like happened. We don't care too mutch about it :)
  m_window->setDebugStatusMsg("Done");
}

void DebuggerManager::slotError(const QString& message)
{
  m_window->showError(message);
}

// void DebuggerManager::slotProfileDialogClosed()
// {
// //   m_showProfileDialog = false;
//   dynamic_cast<KToggleAction*>(m_window->actionCollection()->action("profile"))->setChecked(false);
// }

#include "debuggermanager.moc"
