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

#include "debuggerperldbgp.h"
#include "perldbgpnet.h"
#include "perldbgpsettings.h"

#include "protoeditor.h"
#include "datacontroller.h"
#include "debuggerbreakpoint.h"
#include "debuggerstack.h"
#include "protoeditorsettings.h"
#include "languagesettings.h"

#include <kdebug.h>
#include <klocale.h>


DebuggerPerlDBGP::DebuggerPerlDBGP(LanguageSettings* langs)
    : AbstractDebugger(langs), m_name("perldbgp"), m_isRunning(false), m_isJITActive(false),
      m_listenPort(-1), m_currentExecutionPoint(0), m_globalExecutionPoint(0), 
      m_pdbgSettings(0), m_net(0)
{
  m_pdbgSettings = new PerlDBGPSettings(name(), label(), langSettings());
  langSettings()->registerDebuggerSettings(name(), m_pdbgSettings);

  m_currentExecutionPoint = new DebuggerExecutionPoint();
  m_globalExecutionPoint  = new DebuggerExecutionPoint();


  connect(Protoeditor::self()->settings(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  m_net = new PerlDBGPNet(this);
  connect(m_net, SIGNAL(sigStarted()), this, SLOT(slotStarted()));
  connect(m_net, SIGNAL(sigClosed()), this, SLOT(slotStopped()));
  connect(m_net, SIGNAL(sigError(const QString&)), this, SIGNAL(sigInternalError(const QString&)));
  connect(m_net, SIGNAL(sigStepDone()), this, SLOT(slotStepDone()));
  connect(m_net, SIGNAL(sigNewConnection()), this, SLOT(slotNewConnection()));
  //connect(m_net, SIGNAL(sigBreakpoint()), this, SLOT(slotBreakpoint()));

  slotSettingsChanged();
}

DebuggerPerlDBGP::~DebuggerPerlDBGP()
{
  delete m_pdbgSettings;
  delete m_net;
}

QString DebuggerPerlDBGP::name() const
{
  return m_name;
}

QString DebuggerPerlDBGP::label() const
{
  return i18n("DBGp");
}

PerlDBGPSettings* DebuggerPerlDBGP::settings() {
  return m_pdbgSettings;
}

bool DebuggerPerlDBGP::isRunning() const
{
  return m_isRunning;
}

// void DebuggerPerlDBGP::init()
// {
//   slotSettingsChanged();
// }

void DebuggerPerlDBGP::start(const QString& filepath, const QString& args, bool local)
{
  SiteSettings* site  = Protoeditor::self()->settings()->currentSiteSettings();

  //we need JIT active (listening on port)
  if(!m_isJITActive && !startJIT())
  {
    return;
  }

  emit sigDebugStarting();

  m_net->startDebugging(filepath, args, site, local);
}

void DebuggerPerlDBGP::continueExecution()
{
  if(isRunning())
  {
    m_net->requestContinue();
  }
}

void DebuggerPerlDBGP::stop()
{
  if(isRunning())
  {
    m_net->requestStop();
  }
}

void DebuggerPerlDBGP::runToCursor(const QString& filePath, int line)
{
  if(isRunning())
  {
    m_net->requestRunToCursor(filePath, line);    
  }
}

void DebuggerPerlDBGP::stepInto()
{
  if(isRunning())
  {
    m_net->requestStepInto();
  }
}

void DebuggerPerlDBGP::stepOver()
{
  if(isRunning())
  {
    m_net->requestStepOver();
  }
}

void DebuggerPerlDBGP::stepOut()
{
  if(isRunning())
  {
    m_net->requestStepOut();
  }
}

void DebuggerPerlDBGP::addBreakpoints(const QValueList<DebuggerBreakpoint*>& bps)
{
  if(isRunning())
  {
    QValueList<DebuggerBreakpoint*>::const_iterator it;
    for(it = bps.begin(); it != bps.end(); ++it)
    {
      m_net->requestBreakpoint(*it);
    }
  }
}

void DebuggerPerlDBGP::addBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpoint(bp);
  }
}

void DebuggerPerlDBGP::changeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpointUpdate(bp);
  }
}

void DebuggerPerlDBGP::removeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpointRemoval(bp->id());
  }
}

void DebuggerPerlDBGP::changeCurrentExecutionPoint(DebuggerExecutionPoint* execPoint)
{
  m_currentExecutionPoint = execPoint;
  if(isRunning())
  {
    m_net->requestVariables(m_currentExecutionPoint->id(), PerlDBGPNet::LocalScopeId);
  }
}

void DebuggerPerlDBGP::modifyVariable(Variable* var, DebuggerExecutionPoint* execPoint)
{
  if(isRunning())
  {
    m_net->requestModifyVar(var, execPoint->id());
    requestVars();
  }
}

void DebuggerPerlDBGP::addWatches(const QStringList& list)
{
  m_wathcesList = list;  

  if(isRunning())
  {
    QStringList::iterator it;
    for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it)
    {
      m_net->requestWatch(*it);
    }
  }
}

void DebuggerPerlDBGP::addWatch(const QString& expression)
{
  if(m_wathcesList.find(expression) == m_wathcesList.end())
  {
    m_wathcesList.append(expression);
  }

  if(isRunning())
  {
    m_net->requestWatch(expression);
  }
}

void DebuggerPerlDBGP::removeWatch(const QString& expression)
{
  QStringList::iterator it = m_wathcesList.find(expression);

  if(it != m_wathcesList.end())
  {
    m_wathcesList.remove(it);
  }
}

void DebuggerPerlDBGP::getChildren(int scope, Variable* var)
{
  m_net->requestChildren(scope, var);
}

void DebuggerPerlDBGP::slotSettingsChanged()
{
  if(m_pdbgSettings->languageSettings()->isEnabled() && m_pdbgSettings->enableJIT())
  {
    //do not try to restart if we are already listening on the given port
    if(!m_isJITActive || (m_listenPort != m_pdbgSettings->listenPort()))
    {
      startJIT();
    }
  }
  else
  {
    stopJIT();
  }
}

bool DebuggerPerlDBGP::startJIT()
{
  if(m_isJITActive)
  {
    stopJIT();
  }

  if(m_net->startListener(m_pdbgSettings->listenPort()))
  {
    m_isJITActive = true;
    kdDebug() << "Perl dbgp: Listening on port " << m_pdbgSettings->listenPort() << endl;
    m_listenPort = m_pdbgSettings->listenPort();
  }
  else
  {
    emit sigInternalError(i18n("Unable to listen on port: %1").arg(
                            m_pdbgSettings->listenPort()));
    return false;
  }
  return true;
}

void DebuggerPerlDBGP::stopJIT()
{
  if(m_isRunning)
  {
    m_net->requestStop();
  }

  m_net->stopListener();
  m_isJITActive = false;
}

//net started
void DebuggerPerlDBGP::slotStarted()
{
  m_isRunning = true;
  emit sigDebugStarted(this);
}

//net stopped
void DebuggerPerlDBGP::slotStopped()
{
  m_isRunning = false;
  if(!m_pdbgSettings->enableJIT())
  {
    stopJIT();
  }

  emit sigDebugEnded();
}

void DebuggerPerlDBGP::requestVars()
{
  if(isRunning())
  {
    requestWatches(m_currentExecutionPoint->id());
    m_net->requestVariables(m_currentExecutionPoint->id(), PerlDBGPNet::LocalScopeId);
    m_net->requestVariables(m_globalExecutionPoint->id(), PerlDBGPNet::GlobalScopeId);
  }
}

void DebuggerPerlDBGP::slotStepDone()
{
  requestVars();
  emit sigDebugPaused();
}

void DebuggerPerlDBGP::requestWatches(int ctx_id)
{
  if(isRunning())
  {
    QStringList::iterator it;
    for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it)
    {
      m_net->requestWatch(*it, ctx_id);
    }
  }
}

void DebuggerPerlDBGP::updateStack(DebuggerStack* stack)
{
  m_currentExecutionPoint = stack->topExecutionPoint();
  m_globalExecutionPoint  = stack->bottomExecutionPoint();
  Protoeditor::self()->dataController()->updateStack(stack);

}

void DebuggerPerlDBGP::updateVariables(VariableList_t* array, bool isGlobal)
{
  if(isGlobal)
  {
    Protoeditor::self()->dataController()->updateGlobalVars(array);
  }
  else
  {
    Protoeditor::self()->dataController()->updateLocalVars(array);
  }
}

void DebuggerPerlDBGP::updateWatch(Variable* var)
{
  if(m_wathcesList.find(var->name()) == m_wathcesList.end())
  {
    //This watch is not on our list.
    //It might happen whe the user modifies the value of a variable (ie. "$var=123")
    //through the VariableListView or through evaluation of code.
    //Since, for evaluation and change of variable value,
    //we request a watch expression "$var=123", we receive "$var=123" as the name
    //of the variable and we don't want to add something like that to the watchlist
    //everytime the user modifies a variable, right? :)
    return;
  }

  Protoeditor::self()->dataController()->updateWatch(var);
}

void DebuggerPerlDBGP::updateBreakpoint(int id, const QString& filePath, int line, const QString& state, int hitcount, int skiphits,
                                  const QString& condition)
{
  int status;
  if(state == "enabled")
  {
    status = DebuggerBreakpoint::ENABLED;
  }
  else if(state == "disabled")
  {
    status = DebuggerBreakpoint::DISABLED;
  }
  else
  {
    status = DebuggerBreakpoint::UNRESOLVED;
  }

  DebuggerBreakpoint* bp = new DebuggerBreakpoint(id, KURL::fromPathOrURL(filePath),
       line, status, condition, hitcount, skiphits);

  Protoeditor::self()->dataController()->updateBreakpoint(bp);
}

void DebuggerPerlDBGP::addOutput(const QString& msg)
{
  Protoeditor::self()->dataController()->addOutput(msg);
}

void DebuggerPerlDBGP::debugError(int code, const QString& filePath, int line, const QString& message)
{

//   int line = m_currentExecutionPoint->line();
//   QString filePath = m_currentExecutionPoint->filePath();

  switch(code)
  {
    case ErrorMsg:
      Protoeditor::self()->dataController()->debugMessage(DataController::ErrorMsg, message, KURL::fromPathOrURL(filePath), line);
      break;
    case WarningMsg:
      Protoeditor::self()->dataController()->debugMessage(DataController::WarningMsg, message, KURL::fromPathOrURL(filePath), line);
      break;
    case InfoMsg:
      Protoeditor::self()->dataController()->debugMessage(DataController::InfoMsg, message, KURL::fromPathOrURL(filePath), line);
      break;
  }
}

void DebuggerPerlDBGP::updateVariable(Variable* var, int)
{
  Protoeditor::self()->dataController()->updateVariable(var);
}

void DebuggerPerlDBGP::slotNewConnection()
{
  //we don't know if it is requested session or JIT, so we have
  //to update the m_net site
  m_net->setSite(Protoeditor::self()->settings()->currentSiteSettings());

  emit sigJITStarted(this);
}

#include "debuggerperldbgp.moc"
