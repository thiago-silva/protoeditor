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

#include "debuggermanager.h"
#include "debuggerfactory.h"

#include "abstractdebugger.h"
#include "debuggerstack.h"
#include "debuggerbreakpoint.h"

#include "mainwindow.h"
#include "debuggerui.h"
#include "editorui.h"
#include "statusbarwidget.h"
#include "protoeditor.h"

#include "breakpointlistview.h"
#include "combostack.h"
#include "watchtab.h"
#include "watchlistview.h"
#include "messagelistview.h"

#include "protoeditorsettings.h"
#include "sitesettings.h"
#include "phpsettings.h"

#include <klocale.h>
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
  //connects the EditorUI to DebuggerUI
  connect(m_window->editorUI(), SIGNAL(sigBreakpointMarked(const KURL&, int, bool)),
    m_window->debuggerUI(), SLOT(slotBreakpointMarked(const KURL&, int, bool)));

  connect(m_window->editorUI(), SIGNAL(sigBreakpointUnmarked(const KURL&, int)),
    m_window->debuggerUI(), SLOT(slotBreakpointUnmarked(const KURL&, int)));

  connect(this, SIGNAL(sigDebugStarted()),
      m_window->debuggerUI(), SLOT(slotDebugStarted()));

  connect(this, SIGNAL(sigDebugEnded()),
    m_window->debuggerUI(), SLOT(slotDebugEnded()));

  //connects the WATCH UI to us
  connect(m_window->debuggerUI(), SIGNAL(sigWatchAdded(const QString&)),
          this, SLOT(slotWatchAdded(const QString&)));

  connect(m_window->debuggerUI(), SIGNAL(sigWatchRemoved(Variable*)),
          this, SLOT(slotWatchRemoved(Variable*)));

  connect(m_window->debuggerUI(), SIGNAL(sigLocalVarModified(Variable*)),
          this, SLOT(slotLocalVarModified(Variable*)));

  connect(m_window->debuggerUI(), SIGNAL(sigGlobalVarModified(Variable*)),
          this, SLOT(slotGlobalVarModified(Variable*)));

  connect(m_window->debuggerUI(),
          SIGNAL(sigStackchanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)), this,
          SLOT(slotStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)));


  connect(m_window->editorUI(), SIGNAL(sigNewPage()),
          this, SLOT(slotNewDocument()));

  connect(m_window->editorUI(), SIGNAL(sigEmpty()),
          this, SLOT(slotNoDocument()));


  //connects the BREAKPOINT UI to us
  connect(m_window->debuggerUI(), SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointCreated(DebuggerBreakpoint*)));

  connect(m_window->debuggerUI(), SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointChanged(DebuggerBreakpoint*)));

  connect(m_window->debuggerUI(), SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)),
          this, SLOT(slotBreakpointRemoved(DebuggerBreakpoint*)));

  connect(m_window->debuggerUI(), SIGNAL(sigGotoFileAndLine( const KURL&, int )),
          this, SLOT(slotGotoLineAtFile(const KURL&, int)));

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

  connect(debugger, SIGNAL(sigDebugPaused()),
          this, SLOT(slotDebugPaused()));

  connect(debugger, SIGNAL(sigDebugStarting()),
          this, SLOT(slotDebugStarting()));

  connect(debugger, SIGNAL(sigJITStarted(AbstractDebugger*)),
          this, SLOT(slotJITStarted(AbstractDebugger*)));
}


/******************************* Application interface ******************************************/

void DebuggerManager::slotConfigurationChanged()
{
  loadDebuggers();
}

void DebuggerManager::slotScriptRun()
{
  if(m_window->editorUI()->count() == 0)
  {
    Protoeditor::self()->openFile();
    if(m_window->editorUI()->count() == 0)
    {
      //couldn't open the file for some reason
      return;
    }
  } else if(!m_window->editorUI()->currentDocumentExistsOnDisk()) {
    if(!Protoeditor::self()->saveCurrentFileAs()) {
      //user didn't want to save the current file
      return;
    }
  } else {
    //saves the current file before execute
    Protoeditor::self()->saveCurrentFile();
  }

  if(m_activeDebugger && m_activeDebugger->isRunning()) 
  {
    m_activeDebugger->stop();
  }

  KURL url = m_window->editorUI()->currentDocumentURL();

  if(!url.isLocalFile())
  {
    m_window->showSorry(i18n("Unable to run non-local file"));
  }
  else
  {
    Session::self()->start(url, m_window->argumentCombo()->currentText());
  }
}

KURL DebuggerManager::sessionPrologue(bool isProfiling) 
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
      m_window->statusBar()->setDebugStatusMsg(i18n("Continuing..."));
      return KURL();
    }
  }
  
  KURL url;
  SiteSettings* currentSite = ProtoeditorSettings::self()->currentSiteSettings();
  if(currentSite)
  {
    m_activeDebugger = m_debuggerMap[currentSite->debuggerClient()];   
 
    if(!Protoeditor::self()->useCurrentScript())
    {
      url = currentSite->defaultFile();
    }
  }
  else
  {
    //m_activeDebugger = m_debuggerMap[ProtoeditorSettings::self()->phpSettings()->defaultDebugger()];
  }
  
  
  if(!m_activeDebugger)
  {
    m_window->showSorry(i18n("Error loading debugger."));
    return KURL();
  }

  if(url.isEmpty())
  {
    if(m_window->editorUI()->count() == 0)
    {
      Protoeditor::self()->openFile();
      if(m_window->editorUI()->count() == 0)
      {
        //couldn't open the file for some reason
        return KURL();
      }
    }
    url = m_window->editorUI()->currentDocumentURL();
  }

  if(!url.isLocalFile())
  {
    m_window->showSorry(i18n("Unable to debug non-local file"));
    return KURL();
  }

  return url;
}

void DebuggerManager::processSession(const KURL& url, bool local, bool isProfiling)
{
  if(isProfiling)
  {
    m_activeDebugger->profile(url.path(), m_window->argumentCombo()->currentText(), local);
  }
  else
  {
    m_activeDebugger->start(url.path(), m_window->argumentCombo()->currentText(), local);
  }
}

void DebuggerManager::slotDebugStart()
{
  KURL url = sessionPrologue(false);
  if(!url.isEmpty()) {
    processSession(url, ProtoeditorSettings::self()->currentSiteSettings()?false:true, false);
  }
}

void DebuggerManager::slotProfile()
{
  KURL url = sessionPrologue(true);
  if(!url.isEmpty()) {
    processSession(url, ProtoeditorSettings::self()->currentSiteSettings()?false:true, true);
  }
}

void DebuggerManager::slotDebugStop()
{
  if(!m_activeDebugger) return;

  m_window->statusBar()->setDebugStatusMsg(i18n("Stoping..."));
  m_activeDebugger->stop();
}

void DebuggerManager::slotDebugRunToCursor()
{
  if(!m_activeDebugger) return;

  m_window->statusBar()->setDebugStatusMsg(i18n("Continuing..."));
  m_activeDebugger->runToCursor(
    m_window->editorUI()->currentDocumentURL().path(),
    m_window->editorUI()->currentDocumentLine());
}

void DebuggerManager::slotDebugStepInto()
{
  if(!m_activeDebugger) return;

  m_window->statusBar()->setDebugStatusMsg(i18n("Stepping..."));
  m_activeDebugger->stepInto();
}

void DebuggerManager::slotDebugStepOver()
{
  if(!m_activeDebugger) return;

  m_window->statusBar()->setDebugStatusMsg(i18n("Stepping..."));
  m_activeDebugger->stepOver();
}

void DebuggerManager::slotDebugStepOut()
{
  if(!m_activeDebugger) return;

  m_window->statusBar()->setDebugStatusMsg(i18n("Stepping..."));
  m_activeDebugger->stepOut();
}

//from the menu action
void DebuggerManager::slotDebugToggleBp()
{
  KURL url = m_window->editorUI()->currentDocumentURL();
  int line = m_window->editorUI()->currentDocumentLine();

  if(!m_window->editorUI()->hasBreakpointAt(url, line))
  {
    m_window->editorUI()->markActiveBreakpoint(url, line);
  }
  else
  {
    m_window->editorUI()->unmarkActiveBreakpoint(url, line);
  }

  m_window->debuggerUI()->toggleBreakpoint(url, line);
}

void DebuggerManager::slotWatchAdded(const QString& expression)
{
  if(m_activeDebugger) 
  {
    m_activeDebugger->addWatch(expression);
  }
}

void DebuggerManager::slotStackChanged(DebuggerExecutionPoint* old, DebuggerExecutionPoint* nw)
{
  //Glossary:
  //  -PreExecutionPoint: a DebuggerExecutionPoint representing a point in the stack of the backtrace.
  //                      This is used when the user selects a point on the stack ComboBox that is not the
  //                      topmost (the active line the debugger is running)
  
  
  //-set the current document/line according to the new stack context
  //-unmark the (possibly) previously PreExecutionPoint according to the old stack context
  //-mark the PreExecutionPoint of the new stack context
  //-request the variables for this context

  EditorUI* ed = m_window->editorUI();

  ed->gotoLineAtFile(nw->url(), nw->line()-1);

  ed->unmarkPreExecutionPoint(old->url()/*, old->line()*/);

  if(nw != m_window->debuggerUI()->stack()->topExecutionPoint())
  {
    ed->markPreExecutionPoint(nw->url(), nw->line());
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
                                   m_window->debuggerUI()->stack()->bottomExecutionPoint());
}

void DebuggerManager::slotLocalVarModified(Variable* var)
{
  if(!m_activeDebugger) return;

  m_activeDebugger->modifyVariable(var,
                                   m_window->debuggerUI()->selectedDebuggerExecutionPoint());
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
      m_window->editorUI()->unmarkDisabledBreakpoint(bp->url(), bp->line());
      m_window->editorUI()->markActiveBreakpoint(bp->url(), bp->line());
      break;
    case DebuggerBreakpoint::DISABLED:
      m_window->editorUI()->unmarkActiveBreakpoint(bp->url(), bp->line());
      m_window->editorUI()->markDisabledBreakpoint(bp->url(), bp->line());
    case DebuggerBreakpoint::UNRESOLVED:
      break;
  }
}

void DebuggerManager::slotBreakpointRemoved(DebuggerBreakpoint* bp)
{
  if(bp->status() == DebuggerBreakpoint::ENABLED) {
    m_window->editorUI()->unmarkActiveBreakpoint(
      bp->url(), bp->line());
  } else {
    m_window->editorUI()->unmarkDisabledBreakpoint(
      bp->url(), bp->line());
  }

  if(m_activeDebugger)
  {
    m_activeDebugger->removeBreakpoint(bp);
  }
}

void DebuggerManager::slotGotoLineAtFile(const KURL& url, int line)
{
  m_window->editorUI()->gotoLineAtFile(url, line-1);
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
    m_window->debuggerUI()->breakpointsFrom(
      m_window->editorUI()->currentDocumentURL());

  QValueList<DebuggerBreakpoint*>::iterator it;
  for(it = bplist.begin(); it != bplist.end(); ++it)
  {
    m_window->editorUI()->markActiveBreakpoint((*it)->url(), (*it)->line());
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
  EditorUI* ed = m_window->editorUI();

  if(m_window->debuggerUI()->stack()->count() > 0)
  {
    execPoint =
      m_window->debuggerUI()->stack()->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }

  if(m_window->debuggerUI()->currentStackItem() != 0)
  {
    execPoint =
      m_window->debuggerUI()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }

  //**** dealing with the new Stack (the argument)
  //-set the current document/line according to the top context of the stack
  //-mark the new ExecutionPoint
  //-sets the stack to the comboStack

  //lets update first and force the user to be on the top of the stack
  m_window->debuggerUI()->setStack(stack);

  execPoint = stack->topExecutionPoint();
  ed->setCurrentDocumentTab(execPoint->url(), true);
  ed->gotoLineAtFile(execPoint->url(), execPoint->line()-1);

  ed->markExecutionPoint(execPoint->url(), execPoint->line());
}

void DebuggerManager::updateGlobalVars(VariableList_t* vars)
{
  m_window->debuggerUI()->setGlobalVariables(vars);
}

void DebuggerManager::updateLocalVars(VariableList_t* vars)
{
  m_window->debuggerUI()->setLocalVariables(vars);
}

void DebuggerManager::updateWatch(Variable* var)
{
  m_window->debuggerUI()->addWatch(var);
}

void DebuggerManager::updateBreakpoint(DebuggerBreakpoint* bp)
{
  m_window->debuggerUI()->updateBreakpoint(bp);

  switch(bp->status())
  {
    case DebuggerBreakpoint::ENABLED:
      m_window->editorUI()->unmarkDisabledBreakpoint(bp->url(), bp->line());
      m_window->editorUI()->markActiveBreakpoint(bp->url(), bp->line());
      break;
    case DebuggerBreakpoint::DISABLED:
      m_window->editorUI()->unmarkActiveBreakpoint(bp->url(), bp->line());
      m_window->editorUI()->markDisabledBreakpoint(bp->url(), bp->line());
    case DebuggerBreakpoint::UNRESOLVED:
      break;
  }
}

void DebuggerManager::debugMessage(int type, const QString& msg, const KURL& url, int line)
{
  if(type == DebuggerManager::ErrorMsg) 
  {
    m_window->editorUI()->gotoLineAtFile(url, line);
    m_window->showError(msg);
  }
  m_window->debuggerUI()->addMessage(type, msg, line, url);
}

void DebuggerManager::addOutput(const QString& output)
{
   m_window->debuggerUI()->appendOutput(output);
}

void DebuggerManager::slotDebugStarting()
{
  //m_window->setLedState(MainWindow::LedWait);
  //m_window->setDebugStatusMsg("Starting...");
}

void DebuggerManager::slotDebugStarted(AbstractDebugger* debugger)
{
  emit sigDebugStarted();
  
  m_activeDebugger = debugger;
  
  m_window->statusBar()->setDebugStatusMsg(i18n("Debug started"));
  m_window->statusBar()->setDebugStatusName(m_activeDebugger->name()); //show the name of the debugger on the StatusBar
  m_window->statusBar()->setLedState(StatusBarWidget::LedOn);               //green led

  //setup the actions stuff
  m_window->actionStateChanged("debug_started");
  m_window->actionCollection()->action("debug_start")->setText(i18n("Continue"));

  m_window->actionCollection()->action("site_selection")->setEnabled(false);

  //send all breakpoints to the debugger 
  m_activeDebugger->addBreakpoints(
    m_window->debuggerUI()->breakpoints());

  //send all watches to the debugger
  m_activeDebugger->addWatches(m_window->debuggerUI()->watches());

  //removes any existing pre execution point
  EditorUI* ed = m_window->editorUI();
  DebuggerStack* stack = m_window->debuggerUI()->stack();

  if(!stack->isEmpty())
  {
    DebuggerExecutionPoint* execPoint;
    execPoint =
      m_window->debuggerUI()->selectedDebuggerExecutionPoint();

    ed->unmarkPreExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }
}

void DebuggerManager::slotDebugEnded()
{
  emit sigDebugEnded();

  //setup the actions stuff
  m_window->actionCollection()->action("site_selection")->setEnabled(true);
  m_window->actionStateChanged("debug_stopped");
  m_window->actionCollection()->action("debug_start")->setText(i18n("Start Debug"));
  
  m_window->statusBar()->setDebugStatusMsg(i18n("Stopped"));
  m_window->statusBar()->setLedState(StatusBarWidget::LedOff); //red led

  //removes the current execution point
  EditorUI* ed = m_window->editorUI();
  DebuggerStack* stack = m_window->debuggerUI()->stack();

  if(stack)
  {
    //remove the execution line mark
    DebuggerExecutionPoint* execPoint;
    execPoint = stack->topExecutionPoint();

    ed->unmarkExecutionPoint(execPoint->url()/*, execPoint->line()*/);
  }

  m_activeDebugger = 0;
  m_window->statusBar()->setDebugStatusName("");
}

void DebuggerManager::slotDebugPaused()
{
  //step(into/over/out) or a breakpoint or something like happened. We don't care too mutch about it :)
  m_window->statusBar()->setDebugStatusMsg("Done");
}

void DebuggerManager::slotJITStarted(AbstractDebugger* debugger)
{
  if(m_activeDebugger && (debugger != m_activeDebugger) && m_activeDebugger->isRunning())
  {
    m_activeDebugger->stop();
  }
}

void DebuggerManager::slotError(const QString& message)
{
  m_window->showError(message);
}

#include "debuggermanager.moc"
