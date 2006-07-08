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

#include "perldebugger.h"
#include "perldebuggersettings.h"

#include "protoeditor.h"
#include "datacontroller.h"
#include "perlcom.h"
#include "perllocalcom.h"
#include "protoeditorsettings.h"
#include "perlsettings.h"

#include <klocale.h>
#include <qregexp.h>

PerlDebugger::PerlDebugger(LanguageSettings* langs)
  : AbstractDebugger(langs), m_name("perldb"), m_isRunning(false), m_isJITActive(false)
    , m_listenPort(-1), m_perlCom(0)
{

  m_perlDebuggerSettings = new PerlDebuggerSettings(name(), label(), langSettings());
  langSettings()->registerDebuggerSettings(name(), m_perlDebuggerSettings);

  connect(Protoeditor::self()->settings(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

//   m_currentExecutionPoint = new DebuggerExecutionPoint();
//   m_globalExecutionPoint = new DebuggerExecutionPoint();

  slotSettingsChanged();

}

PerlDebugger::~PerlDebugger()
{
}

QString PerlDebugger::name()   const
{ 
  return m_name;
}

QString PerlDebugger::label()   const
{
  return i18n("Console DB");
}

bool PerlDebugger::isRunning() const
{
  return m_isRunning;
}


void PerlDebugger::init()
{
}


void PerlDebugger::start(const QString& filePath, const QString& args, bool local)
{
  emit sigDebugStarting(); 

  if(!local) 
  {
    //we need JIT active (listening on port)
    if(!m_isJITActive && !startJIT())
    {
      return;
    }
    //TODO!
//     SiteSettings* site  = Protoeditor::self()->settings()->currentSiteSettings();
    emit sigInternalError(i18n("not implemented!"));
  } 
  else
  {
    m_perlCom = new PerlLocalCom();
    connect(m_perlCom, SIGNAL(sigOutput(const QString&)),
      this, SLOT(slotParseOutput(const QString&)));

    m_perlCom->startDebugging(filePath, args);

    emit sigJITStarted(this);
  }
}

void PerlDebugger::continueExecution()
{
  if(isRunning())
  {
    m_perlCom->requestContinue();
  }
}

void PerlDebugger::stop()
{
  if(isRunning())
  {
    m_perlCom->requestStop();
  }
}

void PerlDebugger::runToCursor(const QString& filePath, int line)
{
  if(isRunning())
  {
    m_perlCom->requestRunToCursor(filePath, line);
  }
}

void PerlDebugger::stepInto()
{
  if(isRunning())
  {
    m_perlCom->requestStepInto();
  }
}

void PerlDebugger::stepOver()
{
  if(isRunning())
  {
//     m_perlCom->requestStepOver();
  }
}

void PerlDebugger::stepOut()
{
  if(isRunning())
  {
//     m_perlCom->requestStepOut();
  }
}


void PerlDebugger::addBreakpoints(const QValueList<DebuggerBreakpoint*>& bps)
{
  if(isRunning())
  {
    QValueList<DebuggerBreakpoint*>::const_iterator it;
    for(it = bps.begin(); it != bps.end(); ++it)
    {
      m_perlCom->requestBreakpoint(*it);
    }
  }
}

void PerlDebugger::addBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_perlCom->requestBreakpoint(bp);
  }
}


void PerlDebugger::changeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
//     m_perlCom->requestBreakpointUpdate(bp);
  }
}

void PerlDebugger::removeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
//     m_perlCom->requestBreakpointRemoval(bp->id());
  }
}

void PerlDebugger::changeCurrentExecutionPoint(DebuggerExecutionPoint* bp)
{
//   m_currentExecutionPoint = execPoint;
//   if(isRunning())
//   {
//     m_perlCom->requestVariables(m_currentExecutionPoint->id(), XDNet::LocalScopeId);
//   }
}

void PerlDebugger::modifyVariable(Variable* v, DebuggerExecutionPoint*)
{
  emit sigInternalError("not impl");
}

void PerlDebugger::addWatches(const QStringList&)
{
//   m_wathcesList = list;  
// 
//   if(isRunning())
//   {
//     QStringList::iterator it;
//     for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it)
//     {
//       m_perlCom->requestWatch(*it);
//     }
//   }
}

void PerlDebugger::addWatch(const QString& expression)
{
//   if(m_wathcesList.find(expression) == m_wathcesList.end())
//   {
//     m_wathcesList.append(expression);
//   }
// 
//   if(isRunning())
//   {
//     m_perlCom->requestWatch(expression);
//   }
}

void PerlDebugger::removeWatch(const QString& expression)
{
//   QStringList::iterator it = m_wathcesList.find(expression);
// 
//   if(it != m_wathcesList.end())
//   {
//     m_wathcesList.remove(it);
//   }
}


void PerlDebugger::executeCmd(const QString& cmd)
{
  m_perlCom->executeCmd(cmd);
}

//---------------


void PerlDebugger::slotSettingsChanged()
{
  //if(m_pdbgSettings->languageSettings()->isEnabled() && m_pdbgSettings->enableJIT())
//   if(m_perlDebuggerSettings->enableJIT())
//   {
//     //do not try to restart if we are already listening on the given port
//     if(!m_isJITActive || (m_listenPort != m_perldbgpSettings->listenPort()))
//     {
//       startJIT();
//     }
//   }
//   else
//   {
//     stopJIT();
//   }
}

bool PerlDebugger::startJIT()
{
//   if(m_isJITActive)
//   {
//     stopJIT();
//   }
// 
//   if(m_perlCom->startListener(m_perldbgpSettings->listenPort()))
//   {
//     m_isJITActive = true;
//     kdDebug() << "Perl debugger: Listening on port " << m_perldbgpSettings->listenPort() << endl;
//     m_listenPort = m_perldbgpSettings->listenPort();
//   }
//   else
//   {
//     emit sigInternalError(i18n("Unable to listen on port: %1").arg(
//                             m_perldbgpSettings->listenPort()));
//     return false;
//   }
//   return true;
}

void PerlDebugger::stopJIT()
{
//   if(m_isRunning)
//   {
//     m_perlCom->requestStop();
//   }
// 
//   m_perlCom->stopListener();
//   m_isJITActive = false;
}

void PerlDebugger::slotParseOutput(const QString& output)
{
  //main::(teste.pl:2):     $teste = "thiago";

  QRegExp rx;
  //rx.setPattern(".*\027.*DB<\\d*>.*");
  //rx.setPattern("^(\\w+)::\(\\w+:\\d+\):(\\s+([^\n]*)|\n\\d+:\\s+)");

  //rx.setPattern("(\\w+)::\\(([^:]+:\\d+)\\):(\\s+[^\\n]+)");
  rx.setPattern("(\\n+|^)(\\w+::[^/]*)\\(([^:]+):(\\d+)\\):(\\s+[^\\n]+)");

  /*
    main::(/home/jester/eclipse/workspace/teste/teste.pl:16):\n16:\tsomef();\nDB:"" }
    main::somef(/home/jester/eclipse/workspace/teste/teste.pl:6):\n6:\t\tmy $teste = \"asd asd asd asd as as"" }
  */

  QString topStack, filePath, parse;
  int line;
  if(rx.search(output, 0) != -1) 
  {
    topStack = rx.cap(2);
    filePath = rx.cap(3);
    line    = rx.cap(4).toInt();
    parse = rx.cap(5);
    
    //dispatchStackInfo(
  }

  emit sigConsoleDebuggerOutput(output);
}

#include "perldebugger.moc"
