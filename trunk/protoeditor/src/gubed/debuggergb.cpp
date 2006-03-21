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

#include "debuggergb.h"
#include "debuggermanager.h"
#include "protoeditorsettings.h"

#include "gbsettings.h"
#include "gbnet.h"
#include "phpdefs.h"
#include "debuggerbreakpoint.h"
#include "debuggerstack.h"

#include <kdebug.h>
#include <klocale.h>

#include "phpvariable.h"
#include "phpvariableparser.h"


DebuggerGB::DebuggerGB(DebuggerManager* manager)
    : AbstractDebugger(manager), m_name("Gubed"), m_isRunning(false), m_isJITActive(false),
      m_listenPort(-1), m_currentExecutionPoint(0), m_globalExecutionPoint(0), 
      m_gbSettings(0), m_net(0)
{
  m_currentExecutionPoint = new DebuggerExecutionPoint();
  m_globalExecutionPoint = new DebuggerExecutionPoint();

  m_gbSettings = new GBSettings(m_name);

  ProtoeditorSettings::self()->registerDebuggerSettings(m_gbSettings, m_name);

  connect(ProtoeditorSettings::self(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  m_net = new GBNet(this);
  connect(m_net, SIGNAL(sigGBStarted()), this, SLOT(slotGBStarted()));
  connect(m_net, SIGNAL(sigGBClosed()), this, SLOT(slotGBClosed()));
  connect(m_net, SIGNAL(sigError(const QString&)), this, SIGNAL(sigInternalError(const QString&)));
  connect(m_net, SIGNAL(sigStepDone()), this, SLOT(slotStepDone()));
}


DebuggerGB::~DebuggerGB()
{
  delete m_gbSettings;
  delete m_net;
}

QString DebuggerGB::name() const
{
  return m_name;
}

bool DebuggerGB::isRunning() const
{
  return m_isRunning;
}

void DebuggerGB::init()
{
  slotSettingsChanged();
}


void DebuggerGB::start(const QString& filePath, const QString& args, bool local)
{
  SiteSettings* site  = ProtoeditorSettings::self()->currentSiteSettings();

  //we need JIT active (listening on port)
  if(!m_isJITActive && !startJIT())
  {
    return;
  }

  emit sigDebugStarting();

  m_net->startDebugging(filePath, args, site, local);
}

GBSettings* DebuggerGB::settings()
{
  return m_gbSettings;
}

void DebuggerGB::continueExecution()
{
  if(isRunning())
    m_net->requestContinue();
}

void DebuggerGB::stop()
{
  if(isRunning())
    m_net->requestStop();
}

void DebuggerGB::stepInto()
{
  if(isRunning())
    m_net->requestStepInto();
}

void DebuggerGB::stepOver()
{
  if(isRunning())
    m_net->requestStepOver();
}

void DebuggerGB::stepOut()
{
  if(isRunning())
    m_net->requestStepOut();
}

void DebuggerGB::addBreakpoints(const QValueList<DebuggerBreakpoint*>& bps)
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

void DebuggerGB::addBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpoint(bp);
  }
}

void DebuggerGB::changeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpointUpdate(bp);
  }
}

void DebuggerGB::removeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpointRemoval(bp);
  }
}

void DebuggerGB::changeCurrentExecutionPoint(DebuggerExecutionPoint*)
{
  //
}

void DebuggerGB::modifyVariable(Variable* var, DebuggerExecutionPoint*)
{
  if(isRunning())
  {
    QString name  =  "$" + var->compositeName();
    QString value =  var->value()->toString();

    if(value.isEmpty()) value = "null";

    m_net->requestChangeVar(name, value);

    //reload variables (global/local/watches) to get the new value.
//     requestVars();
  }
}


void DebuggerGB::addWatches(const QStringList& list)
{
  m_wathcesList = list;  

  if(isRunning())
  {
    m_net->requestWatches(list);
  }
}

void DebuggerGB::addWatch(const QString& expression)
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

void DebuggerGB::removeWatch(const QString& expression)
{
  QStringList::iterator it = m_wathcesList.find(expression);

  if(it != m_wathcesList.end())
  {
    m_wathcesList.remove(it);
  }
}

void DebuggerGB::profile(const QString&, const QString&, bool)
{}


void DebuggerGB::slotSettingsChanged()
{
  if(m_gbSettings->enableJIT())
  {
    //do not try to restart if we are already listening on the given port
    if(!m_isJITActive || (m_listenPort != m_gbSettings->listenPort()))
    {
      startJIT();
    }
  }
  else
  {
    stopJIT();
  }
}

void DebuggerGB::slotGBStarted()
{
  m_isRunning = true;
  emit sigDebugStarted(this);
}

void DebuggerGB::slotGBClosed()
{
  m_isRunning = false;
  if(!m_gbSettings->enableJIT())
  {
    stopJIT();
  }

  emit sigDebugEnded();  
}

bool DebuggerGB::startJIT()
{
  if(m_isJITActive)
  {
    stopJIT();
  }

  if(m_net->startListener(m_gbSettings->listenPort()))
  {
    m_isJITActive = true;
    kdDebug() << "Gubed: Listening on port " << m_gbSettings->listenPort() << endl;
    m_listenPort = m_gbSettings->listenPort();
  }
  else
  {
    emit sigInternalError(i18n("Unable to listen on port: %1").arg(
                            m_gbSettings->listenPort()));
    return false;
  }
  return true;
}

void DebuggerGB::stopJIT()
{
  if(m_isRunning)
  {
    m_net->requestStop();
  }

  m_net->stopListener();
  m_isJITActive = false;
}

void DebuggerGB::slotStepDone()
{
  requestVars();
}

void DebuggerGB::requestVars()
{
  //globals
  m_net->requestGlobals();

  //locals
  m_net->requestWatch("$GLOBALS");

  //watches
  m_net->requestWatches(m_wathcesList);  
}

void DebuggerGB::updateStack(DebuggerStack* stack)
{
//   m_currentExecutionPoint = stack->topExecutionPoint();
//   m_globalExecutionPoint  = stack->bottomExecutionPoint();
  manager()->updateStack(stack);

  emit sigDebugBreak(); 
}

void DebuggerGB::updateGlobalVariables(const QString& vars)
{
  PHPVariableParser p(vars);
  VariablesList_t* array = p.parseAnonymousArray();    
  manager()->updateGlobalVars(array);
}

void DebuggerGB::updateLocalVariables(const QString& scope, const QString& vars)
{
  if(scope == "Current Scope") {
    PHPVariableParser p(vars);
    VariablesList_t* array = p.parseAnonymousArray();    
    manager()->updateLocalVars(array);
  }
}

void DebuggerGB::updateWatch(const QString& name, const QString& value)
{
  if(m_wathcesList.find(name) == m_wathcesList.end())
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

  PHPVariable* var;
  if(value == "-") 
  {
    var = new PHPVariable(name);
    PHPScalarValue* val = new PHPScalarValue(var);
    var->setValue(val);
  }
  else
  {
    PHPVariableParser p(value);
    var = p.parseVariable();
    var->setName(name);
  }
  manager()->updateWatch(var);
}

void DebuggerGB::updateMessage(int type, const QString& msg, const QString& filePath, int line)
{
  QString fpath;
  if(filePath != "N") {
    fpath = filePath;
  }

  switch(type)
  {
    case E_ERROR:
    case E_CORE_ERROR:
    case E_PARSE:
    case E_COMPILE_ERROR:
    case E_USER_ERROR:
      manager()->debugMessage(DebuggerManager::ErrorMsg, msg, fpath, line);      
      break;

    case E_WARNING:
    case E_CORE_WARNING:
    case E_COMPILE_WARNING:
    case E_USER_WARNING:
      manager()->debugMessage(DebuggerManager::WarningMsg, msg, fpath, line);
      break;
    case E_NOTICE:
    case E_USER_NOTICE:
    case E_STRICT:
      manager()->debugMessage(DebuggerManager::InfoMsg, msg, fpath, line);
      break;
  }
}

void DebuggerGB::updateError(const QString& filePath)
{
  manager()->debugMessage(DebuggerManager::ErrorMsg, "Fatal error", filePath, 0);  
}

#include "debuggergb.moc"
