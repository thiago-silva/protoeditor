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

#include "debuggerdbg.h"
#include "debuggermanager.h"
#include "dbgconfiguration.h"
#include "dbgnet.h"
#include "dbg_defs.h"
#include "variableparser.h"
#include "phpvariable.h"
#include "debuggerstack.h"
#include "debuggersettings.h"

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>

DebuggerDBG::DebuggerDBG(DebuggerManager* parent)
    : AbstractDebugger(parent), m_isSessionActive(false), m_isRunning(false),
    m_configuration(0), m_net(0)

{
  m_configuration = new DBGConfiguration(
                      DebuggerSettings::localBaseDir(),
                      DebuggerSettings::serverBaseDir(),
                      DebuggerSettings::listenPort(),
                      DebuggerSettings::serverHost());

  m_net = new DBGNet(this);

  connect(m_net, SIGNAL(sigDBGStarted()), this, SLOT(slotDBGStarted()));
  connect(m_net, SIGNAL(sigDBGClosed()), this, SLOT(slotDBGClosed()));
  connect(m_net, SIGNAL(sigError(const QString&)), this, SLOT(slotInternalError(const QString&)));
}

DebuggerDBG::~DebuggerDBG()
{
  delete m_net;
  delete m_configuration;
}

QString DebuggerDBG::name() const
{
  return QString("DBG");
}

int DebuggerDBG::id() const
{
  return DebuggerSettings::EnumClient::DBG;
}

bool DebuggerDBG::isSessionActive() const
{
  return m_isSessionActive;
}

bool DebuggerDBG::isRunning() const
{
  return m_isRunning;
}

void DebuggerDBG::reloadConfiguration()
{
  m_configuration->setLocalBaseDir(DebuggerSettings::localBaseDir());
  m_configuration->setServerBaseDir(DebuggerSettings::serverBaseDir());
  m_configuration->setListenPort(DebuggerSettings::listenPort());
  m_configuration->setServerHost(DebuggerSettings::serverHost());
}

void DebuggerDBG::startSession()
{
  if(!m_isSessionActive) {

    if(m_net->startListener(m_configuration->listenPort())) {
      m_isSessionActive = true;
      kdDebug() << "DBG: listening on port " << m_configuration->listenPort() << endl;
      emit sigSessionStarted();
    } else {
      emit sigInternalError(i18n("Unable to listen on port: %1").arg(
                              m_configuration->listenPort()));
    }
  }
}

void DebuggerDBG::endSession()
{

  if(m_isRunning) {
    m_net->requestStop();
  }

  m_net->stopListener();
  emit sigSessionEnded();
  m_isSessionActive = false;
}

void DebuggerDBG::run(const QString& filepath)
{
  if(!isRunning()) {
    dbgint sessionid = kapp->random();

    QString f = filepath;
    m_net->requestPage(m_configuration->serverHost(),
                       f.remove(m_configuration->localBaseDir()),
                       m_configuration->listenPort(),
                       sessionid);
  } else {
    m_net->requestContinue();
  }
}

void DebuggerDBG::stop()
{
  if(isRunning()) {
    m_net->requestStop();
  }
}

void DebuggerDBG::stepInto()
{
  if(isRunning()) {
    m_net->requestStepInto();
  }
}

void DebuggerDBG::stepOver()
{
  if(isRunning()) {
    m_net->requestStepOver();
  }
}

void DebuggerDBG::stepOut()
{
  if(isRunning()) {
    m_net->requestStepOut();
  }
}

void DebuggerDBG::addBreakpoints(const QValueList<DebuggerBreakpoint*>&)
{
}

void DebuggerDBG::addBreakpoint(DebuggerBreakpoint*)
{}

void DebuggerDBG::addBreakpoint(const QString&, int)
{}

void DebuggerDBG::changeBreakpoint(DebuggerBreakpoint*)
{}

void DebuggerDBG::removeBreakpoint(DebuggerBreakpoint*)
{}


void DebuggerDBG::modifyVariable(Variable* var, DebuggerExecutionPoint* execPoint)
{
  if(isRunning()) {
    QString name  =  var->compositeName();
    QString value = var->value()->toString();

    if(value.isEmpty()) value = "null";

    m_net->requestWatch(name + "=" + value, execPoint->id());

    //reload variables to get the new value
    m_net->requestGlobalVariables();
    m_net->requestLocalVariables(execPoint->id());
  }
}

void DebuggerDBG::requestLocalVariables(DebuggerExecutionPoint* execPoint)
{
  if(isRunning()) {
    m_net->requestLocalVariables(execPoint->id());
  }
}

void DebuggerDBG::addWatch(const QString& expression, DebuggerExecutionPoint* execPoint)
{
  m_wathcesList.append(expression);

  if(isRunning()) {
    m_net->requestWatch(expression, execPoint->id());
  }
}

void DebuggerDBG::removeWatch(const QString& expression)
{
  QValueList<QString>::iterator it = m_wathcesList.find(expression);

  if(it != m_wathcesList.end()) {
    m_wathcesList.remove(it);
  }
}

void DebuggerDBG::requestWatches(DebuggerExecutionPoint* execPoint)
{
  if(isRunning()) {
    QValueList<QString>::iterator it;
    for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it) {
      m_net->requestWatch(*it, execPoint->id());
    }
  }
}

/**************************** DBGNET **************************/

void DebuggerDBG::updateStack(DebuggerStack* stack)
{
  manager()->updateStack(stack);
}

void DebuggerDBG::updateVar(const QString& result, const QString& str, const QString& error, long scope)
{
  if(str.isEmpty()) {
    //global vars
    VariableParser p(result);

    VariablesList_t* array = p.parseAnonymousArray();

    if(scope == GLOBAL_SCOPE_ID) {
      manager()->updateGlobalVars(array);
    } else {
      manager()->updateLocalVars(array);
    }
  } else {
    kdDebug() << "Buggy!! Shouldn't pass here...\n";
  }

  /* We don't want to irritate the user showing errors of watch because var "X" wasn't declared
     He will know on the WatchList the value "Undefined" anyway
  if(error)
  {
    manager()->debugError(error);
  }
  */
}

void DebuggerDBG::updateWatch(const QString& result, const QString& str, const QString& error)
{
  if(m_wathcesList.find(str) == m_wathcesList.end()) {
    //This watch is not on our list.
    //It might happen whe the user modifies the value of a variable (ie. "$var=123").
    //Since we request a watch expression "$var=123", we receive "$var=123" as the name
    //of the variable (str) and we don't want to add something like that to the watchlist
    //everytime the user modifies a variable, right? :)
    return;
  }

  PHPVariable* var;
  if(!result.isEmpty()) {
    VariableParser p(result);

    var = p.parseVariable();
    var->setName(str);
  } else {
    var = new PHPVariable(str);
    PHPScalarValue* value = new PHPScalarValue(var);
    var->setValue(value);
  }

  manager()->updateWatch(var);
}


/***************************** SLOTS *********************************/

void DebuggerDBG::slotInternalError(const QString& msg)
{
  emit sigInternalError(msg);
}

void DebuggerDBG::slotDBGStarted()
{

  //note: erase-me! testing:
  //m_net->requestOptions(SOF_SEND_OUTPUT_DETAILED | SOF_SEND_ERRORS | SOF_BREAKONLOAD);
  m_net->requestOptions(SOF_SEND_OUTPUT_DETAILED | SOF_SEND_ERRORS);

  m_isRunning = true;
  emit sigDebugStarted();
}

void DebuggerDBG::slotDBGClosed()
{
  //end of debug

  m_isRunning = false;
  emit sigDebugEnded();
}

#include "debuggerdbg.moc"
