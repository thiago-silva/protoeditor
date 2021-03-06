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

#include "debuggerxd.h"
#include "xdnet.h"
#include "xdsettings.h"

#include "debuggerbreakpoint.h"
#include "debuggerstack.h"
#include "debuggermanager.h"
#include "protoeditorsettings.h"

#include "phpvariable.h"

#include <kdebug.h>
#include <klocale.h>
#include "phpdefs.h"

DebuggerXD::DebuggerXD(DebuggerManager* manager)
    : AbstractDebugger(manager), m_name("Xdebug"), m_isRunning(false), m_isJITActive(false),
      m_listenPort(-1), m_currentExecutionPoint(0), m_globalExecutionPoint(0), 
      m_xdSettings(0), m_net(0)
{
  m_currentExecutionPoint = new DebuggerExecutionPoint();
  m_globalExecutionPoint = new DebuggerExecutionPoint();

  m_xdSettings = new XDSettings(m_name);

  ProtoeditorSettings::self()->registerDebuggerSettings(m_xdSettings, m_name);

  connect(ProtoeditorSettings::self(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  m_net = new XDNet(this);
  connect(m_net, SIGNAL(sigXDStarted()), this, SLOT(slotXDStarted()));
  connect(m_net, SIGNAL(sigXDClosed()), this, SLOT(slotXDStopped()));
  connect(m_net, SIGNAL(sigError(const QString&)), this, SIGNAL(sigInternalError(const QString&)));
  connect(m_net, SIGNAL(sigStepDone()), this, SLOT(slotStepDone()));
  connect(m_net, SIGNAL(sigNewConnection()), this, SLOT(slotNewConnection()));
  //connect(m_net, SIGNAL(sigBreakpoint()), this, SLOT(slotBreakpoint()));
}

DebuggerXD::~DebuggerXD()
{
  delete m_xdSettings;
  delete m_net;
}


QString DebuggerXD::name() const
{
  return m_name;
}

XDSettings* DebuggerXD::settings() {
  return m_xdSettings;
}

bool DebuggerXD::isRunning() const
{
  return m_isRunning;
}

void DebuggerXD::init()
{
  slotSettingsChanged();
}

void DebuggerXD::start(const QString& filepath, const QString& args, bool local)
{
  SiteSettings* site  = ProtoeditorSettings::self()->currentSiteSettings();

  //we need JIT active (listening on port)
  if(!m_isJITActive && !startJIT())
  {
    return;
  }

  emit sigDebugStarting();

  m_net->startDebugging(filepath, args, site, local);
}

void DebuggerXD::continueExecution()
{
  if(isRunning())
    m_net->requestContinue();
}

void DebuggerXD::stop()
{
  if(isRunning())
    m_net->requestStop();
}

void DebuggerXD::stepInto()
{
  if(isRunning())
    m_net->requestStepInto();
}

void DebuggerXD::stepOver()
{
  if(isRunning())
    m_net->requestStepOver();
}

void DebuggerXD::stepOut()
{
  if(isRunning())
    m_net->requestStepOut();
}

void DebuggerXD::addBreakpoints(const QValueList<DebuggerBreakpoint*>& bps)
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

void DebuggerXD::addBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpoint(bp);
  }
}

void DebuggerXD::changeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpointUpdate(bp);
  }
}

void DebuggerXD::removeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpointRemoval(bp->id());
  }
}

void DebuggerXD::changeCurrentExecutionPoint(DebuggerExecutionPoint* execPoint)
{
  m_currentExecutionPoint = execPoint;
  if(isRunning())
  {
    m_net->requestVariables(m_currentExecutionPoint->id(), XDNet::LocalScopeId);
  }
}

void DebuggerXD::modifyVariable(Variable* var, DebuggerExecutionPoint* execPoint)
{
  if(isRunning())
  {
    QString name  =  var->compositeName();
    QString value =  var->value()->toString();

    if(value.isEmpty()) value = "null";

    m_net->requestWatch(name + "=" + value, execPoint->id());

    //reload variables (global/local/watches) to get the new value.
    requestVars();
  }
}

void DebuggerXD::addWatches(const QStringList& list)
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

void DebuggerXD::addWatch(const QString& expression)
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

void DebuggerXD::removeWatch(const QString& expression)
{
  QStringList::iterator it = m_wathcesList.find(expression);

  if(it != m_wathcesList.end())
  {
    m_wathcesList.remove(it);
  }
}

void DebuggerXD::profile(const QString&, const QString&, bool)
{
  /**/
}

void DebuggerXD::slotSettingsChanged()
{
  if(m_xdSettings->enableJIT())
  {
    //do not try to restart if we are already listening on the given port
    if(!m_isJITActive || (m_listenPort != m_xdSettings->listenPort()))
    {
      startJIT();
    }
  }
  else
  {
    stopJIT();
  }
}

bool DebuggerXD::startJIT()
{
  if(m_isJITActive)
  {
    stopJIT();
  }

  if(m_net->startListener(m_xdSettings->listenPort()))
  {
    m_isJITActive = true;
    kdDebug() << "Xdebug: Listening on port " << m_xdSettings->listenPort() << endl;
    m_listenPort = m_xdSettings->listenPort();
  }
  else
  {
    emit sigInternalError(i18n("Unable to listen on port: %1").arg(
                            m_xdSettings->listenPort()));
    return false;
  }
  return true;
}

void DebuggerXD::stopJIT()
{
  if(m_isRunning)
  {
    m_net->requestStop();
  }

  m_net->stopListener();
  m_isJITActive = false;
}

//net started
void DebuggerXD::slotXDStarted()
{
  m_isRunning = true;
  emit sigDebugStarted(this);
}

//net stopped
void DebuggerXD::slotXDStopped()
{
  m_isRunning = false;
  if(!m_xdSettings->enableJIT())
  {
    stopJIT();
  }

  emit sigDebugEnded();
}

void DebuggerXD::requestVars()
{
  if(isRunning())
  {
    requestWatches(m_currentExecutionPoint->id());
    m_net->requestVariables(m_currentExecutionPoint->id(), XDNet::LocalScopeId);
    m_net->requestVariables(m_globalExecutionPoint->id(), XDNet::GlobalScopeId);
  }
}

void DebuggerXD::slotStepDone()
{
  requestVars();
  emit sigDebugPaused();
}

void DebuggerXD::requestWatches(int ctx_id)
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

void DebuggerXD::updateStack(DebuggerStack* stack)
{
  m_currentExecutionPoint = stack->topExecutionPoint();
  m_globalExecutionPoint  = stack->bottomExecutionPoint();
  manager()->updateStack(stack);

}

void DebuggerXD::updateVariables(VariablesList_t* array, bool isGlobal)
{
  if(isGlobal)
  {
    manager()->updateGlobalVars(array);
  }
  else
  {
    manager()->updateLocalVars(array);
  }
}

void DebuggerXD::updateWatch(Variable* var)
{
  if(m_wathcesList.find(var->name()) == m_wathcesList.end())
  {
    //This watch is not on our list.
    //It might happen whe the user modifies the value of a variable (ie. "$var=123")
    //through the VariablesListView or through evaluation of code.
    //Since, for evaluation and change of variable value,
    //we request a watch expression "$var=123", we receive "$var=123" as the name
    //of the variable and we don't want to add something like that to the watchlist
    //everytime the user modifies a variable, right? :)
    return;
  }

  manager()->updateWatch(var);
}

void DebuggerXD::updateBreakpoint(int id, const QString& filePath, int line, const QString& state, int hitcount, int skiphits,
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

  manager()->updateBreakpoint(bp);
}

void DebuggerXD::addOutput(const QString& msg)
{
  manager()->addOutput(msg);
}

void DebuggerXD::debugError(int code, const QString& filePath, int line, const QString& message)
{
//   int line = m_currentExecutionPoint->line();
//   QString filePath = m_currentExecutionPoint->filePath();

  switch(code)
  {
    case E_ERROR:
    case E_CORE_ERROR:
    case E_PARSE:
    case E_COMPILE_ERROR:
    case E_USER_ERROR:
      manager()->debugMessage(DebuggerManager::ErrorMsg, message, KURL::fromPathOrURL(filePath), line);
      break;

    case E_WARNING:
    case E_CORE_WARNING:
    case E_COMPILE_WARNING:
    case E_USER_WARNING:
      manager()->debugMessage(DebuggerManager::WarningMsg, message, KURL::fromPathOrURL(filePath), line);
      break;
    case E_NOTICE:
    case E_USER_NOTICE:
    case E_STRICT:
      manager()->debugMessage(DebuggerManager::InfoMsg, message, KURL::fromPathOrURL(filePath), line);
      break;
  }
}

void DebuggerXD::slotNewConnection()
{
  //we don't know if it is requested session or JIT, so we have
  //to update the m_net site
  m_net->setSite(ProtoeditorSettings::self()->currentSiteSettings());

  //tell debuggermanager to cleanup any pending session
  emit sigJITStarted(this);
}

#include "debuggerxd.moc"
