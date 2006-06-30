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

#include "executioncontroller.h"
#include "protoeditor.h"
#include "mainwindow.h"
#include "statusbarwidget.h"
#include "editorui.h"
#include "debuggerui.h"

#include "session.h"

#include "protoeditorsettings.h"
#include "languagesettings.h"
#include "sitesettings.h"

#include "abstractdebugger.h"
#include "debuggerfactory.h"

#include "debuggerstack.h"

#include <kurl.h>
#include <klocale.h>


ExecutionController::ExecutionController()
  : m_activeDebugger(0), m_debuggerFactory(0)
{
  
}

ExecutionController::~ExecutionController()
{
}

void ExecutionController::init()
{
  m_debuggerFactory = new DebuggerFactory();
}

QString ExecutionController::currentDebuggerName()
{
  if(m_activeDebugger) 
  {
    return m_activeDebugger->name();
  }
  else
  {
    return QString::null;
  }
}

void ExecutionController::sessionPrologue()
{
  //saves the argument in the history
  Protoeditor::self()->mainWindow()->saveArgumentList();

  //save all opened+existing files
  Protoeditor::self()->mainWindow()->editorUI()->saveExistingFiles();
}

bool ExecutionController::checkForOpenedFile()
{
  //check for opened+existing+saved file
  if(Protoeditor::self()->mainWindow()->editorUI()->count() == 0)
  {
    Protoeditor::self()->openFile();
    if(Protoeditor::self()->mainWindow()->editorUI()->count() == 0)
    {
      //couldn't open the file for some reason
      return false;
    }
  } else if(!Protoeditor::self()->mainWindow()->editorUI()->currentDocumentExistsOnDisk()) {
    if(!Protoeditor::self()->saveCurrentFileAs()) {
      //user didn't want to save the current file
      return false;
    }
  }

  //check if file is in the local system
  if(!Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL().isLocalFile())
  {
    Protoeditor::self()->showSorry(i18n("Unable to run/debug non-local file"));
    return false;
  }

  return true;
}

void ExecutionController::executeScript(const QString& langName, const QString& args)
{
  if(!checkForOpenedFile()) return;
  sessionPrologue();

  KURL url = Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL();

  QString cmd = Protoeditor::self()->settings()->languageSettings(langName)->interpreterCommand();

  Protoeditor::self()->session()->startLocal(langName, url, args);  
}



bool ExecutionController::debugPrologue(const QString& debuggerName, bool willProfile) 
{
  //1: check if there is a debug session running
  //  -if yes, send a continueExecution()
  //1a: If it is about to profile, send stop()
  //2: check the current Site
  //  -if exists, extract the debugger and default file
  //  -else, active debugger is the default debugger in settings
  //3: if no have active debugger so far, emit error
  //4: check if filePath (site's 'default file') is empty
  //  if yes, get filePath from current document / or open a fileDialog
  //  if no filePath so far, return and and we are done.
  //5: debugger->start(siteSettings, filePath);
  
  //TODO: check for any session running
  //if(hasActiveSession()) { kill session? ask to kill before kill?; }


  if(m_activeDebugger && m_activeDebugger->isRunning())
  {
    if(willProfile)
    {
      m_activeDebugger->stop();
    }
    else
    {
      m_activeDebugger->continueExecution();
      Protoeditor::self()->mainWindow()->statusBar()->setDebugMsg(i18n("Continuing..."));
      return false;
    }
  }
  
  SiteSettings* currentSite = Protoeditor::self()->settings()->currentSiteSettings();
  if(currentSite)
  {
    m_activeDebugger = m_debuggerFactory->getDebugger(currentSite->debuggerName());
 
    if(!Protoeditor::self()->useCurrentScript())
    {
      if(currentSite->defaultFile().isEmpty())
      {
        if(!checkForOpenedFile())
        {
          //could not in any way have a opened file to debug
          return false;
        }
      }
      else
      {
        Protoeditor::self()->openFile(currentSite->defaultFile());
      }
    }
    else
    {
      Protoeditor::self()->openFile(currentSite->defaultFile());
    }
  }
  else
  {
    m_activeDebugger = m_debuggerFactory->getDebugger(debuggerName);
  }  
  
  if(!m_activeDebugger)
  {
    Protoeditor::self()->mainWindow()->showSorry(i18n("Error loading debugger."));
    return false;
  }

  return true;
}

void ExecutionController::debugStart(const QString& langName, const QString& args, bool isLocalDebug)
{
  if(!debugPrologue(langName, false)) return;

  sessionPrologue();

  m_activeDebugger->start(Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL().path(), args, isLocalDebug);
}

void ExecutionController::profileScript(const QString& langName, const QString& args, bool isLocalDebug)
{
  if(!debugPrologue(langName, true)) return;

  sessionPrologue();

  m_activeDebugger->profile(Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL().path(), args, isLocalDebug);
}

void ExecutionController::debugStop()
{
  if(m_activeDebugger)
  {
    Protoeditor::self()->mainWindow()->statusBar()->setDebugMsg(i18n("Stopping..."));
    m_activeDebugger->stop();
  }
}

void ExecutionController::debugRunToCursor()
{
  if(m_activeDebugger)
  {
    Protoeditor::self()->mainWindow()->statusBar()->setDebugMsg(i18n("Continuing..."));

    m_activeDebugger->runToCursor(
      Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL().path(),
      Protoeditor::self()->mainWindow()->editorUI()->currentDocumentLine());
  }
}


void ExecutionController::debugStepOver()
{
  if(m_activeDebugger)
  {
     Protoeditor::self()->mainWindow()->statusBar()->setDebugMsg(i18n("Stepping..."));
    m_activeDebugger->stepOver();
  }
}

void ExecutionController::debugStepInto()
{
  if(m_activeDebugger)
  {
    Protoeditor::self()->mainWindow()->statusBar()->setDebugMsg(i18n("Stepping..."));
    m_activeDebugger->stepInto();
  }
}

void ExecutionController::debugStepOut()
{
  if(m_activeDebugger)
  {
    Protoeditor::self()->mainWindow()->statusBar()->setDebugMsg(i18n("Stepping..."));
    m_activeDebugger->stepOut();
  }
}

void ExecutionController::modifyGlobalVariable(Variable* var)
{
  if(m_activeDebugger)
  {
    m_activeDebugger->modifyVariable(
      var, Protoeditor::self()->mainWindow()->debuggerUI()->stack()->bottomExecutionPoint());
  }
}

void ExecutionController::modifyLocalVariable(Variable* var)
{
  if(m_activeDebugger)
  {
    m_activeDebugger->modifyVariable(
      var, Protoeditor::self()->mainWindow()->debuggerUI()->selectedDebuggerExecutionPoint());
  }
}


void ExecutionController::addWatch(const QString& expression)
{
  if(m_activeDebugger) 
  {
    m_activeDebugger->addWatch(expression);
  }
}

void ExecutionController::removeWatch(Variable* var)
{
  if(m_activeDebugger) 
  {
    m_activeDebugger->removeWatch(var->name());
  }  
}

void ExecutionController::addBreakpoint(DebuggerBreakpoint* bp)
{
  if(m_activeDebugger) 
  {
    m_activeDebugger->addBreakpoint(bp);
  }
}

void ExecutionController::changeBreakpoint(DebuggerBreakpoint* bp)
{
  if(m_activeDebugger) 
  {
    m_activeDebugger->changeBreakpoint(bp);
  }
}

void ExecutionController::removeBreakpoint(DebuggerBreakpoint* bp)
{
  if(m_activeDebugger)
  {
    m_activeDebugger->removeBreakpoint(bp);
  }
}

void ExecutionController::changeCurrentExecutionPoint(DebuggerExecutionPoint* execpoint)
{
  if(m_activeDebugger)
  {
    m_activeDebugger->changeCurrentExecutionPoint(execpoint);
  }
}


void ExecutionController::slotDebugStarted(AbstractDebugger* debugger)
{
  emit sigDebugStarted();
  
  m_activeDebugger = debugger;
  
  //send all breakpoints to the debugger 
  m_activeDebugger->addBreakpoints(
    Protoeditor::self()->mainWindow()->debuggerUI()->breakpoints());

  //send all watches to the debugger
  m_activeDebugger->addWatches(Protoeditor::self()->mainWindow()->debuggerUI()->watches());
}

void ExecutionController::slotDebugEnded()
{
  emit sigDebugEnded();
  m_activeDebugger = 0;
}

void ExecutionController::slotDebugPaused()
{
  //step(into/over/out) or a breakpoint or something like happened. We don't care too mutch about it :)
  Protoeditor::self()->mainWindow()->statusBar()->setDebugMsg("Done");
}

void ExecutionController::slotJITStarted(AbstractDebugger* debugger)
{
  if(m_activeDebugger && (debugger != m_activeDebugger) && m_activeDebugger->isRunning())
  {
    m_activeDebugger->stop();
  }
}

#include "executioncontroller.moc"
