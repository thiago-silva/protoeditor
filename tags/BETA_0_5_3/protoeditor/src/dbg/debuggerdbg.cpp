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
#include "debuggerbreakpoint.h"
#include "debuggersettings.h"
#include "phpdefs.h"

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>

#include <qregexp.h>

DebuggerDBG::DebuggerDBG(DebuggerManager* parent)
    : AbstractDebugger(parent), m_isSessionActive(false), m_isRunning(false),
    m_configuration(0), m_net(0), m_currentExecutionPoint(0)

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
  connect(m_net, SIGNAL(sigStepDone()), this, SLOT(slotStepDone()));
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

    m_net->requestPage(m_configuration->serverHost(),
                       filepath,
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

void DebuggerDBG::addBreakpoints(const QValueList<DebuggerBreakpoint*>& bps)
{
  if(isRunning()) {
    QValueList<DebuggerBreakpoint*>::const_iterator it;
    for(it = bps.begin(); it != bps.end(); ++it) {
      m_net->requestBreakpoint(*it);
    }
  }
}

void DebuggerDBG::addBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning()) {
    m_net->requestBreakpoint(bp);
  }
}

/*
void DebuggerDBG::addBreakpoint(const QString&, int)
{}
*/

void DebuggerDBG::changeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning()) {
    m_net->requestBreakpoint(bp);
  }
}

void DebuggerDBG::removeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning()) {
    m_net->requestBreakpointRemoval(bp->id());
  }
}


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
    requestWatches(execPoint);
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
  m_currentExecutionPoint = stack->topExecutionPoint();
  manager()->updateStack(stack);
}

void DebuggerDBG::updateVar(const QString& result, const QString& str, long scope)
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
  }
}

void DebuggerDBG::updateWatch(const QString& result, const QString& str)
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

void DebuggerDBG::updateBreakpoint(int id, const QString& filePath, int line, int state, int hitcount, int skiphits,
                               const QString& condition)
{
  int status;
  switch(state) {
    //case BPS_DELETED
    case BPS_DISABLED:
      status = DebuggerBreakpoint::DISABLED;
      break;
    case BPS_ENABLED:
      status = DebuggerBreakpoint::ENABLED;
      break;
    case BPS_UNRESOLVED:
    default:
      status = DebuggerBreakpoint::UNRESOLVED;
      break;
  }
  DebuggerBreakpoint* bp = new DebuggerBreakpoint(id, filePath, line, status, condition, hitcount, skiphits);
  manager()->updateBreakpoint(bp);
}


void DebuggerDBG::debugError(/*int type,*/ const QString&)
{
  //NOTE: When a PHP error ocurr, I'm not sure why DBG some times
  //sends a TagError and some times not. So, we are sending errors through the Log
  //manager()->debugError(msg);
}

void DebuggerDBG::debugLog(int type, const QString& msg, int line, const QString& filePath, int extInfo)
{
  QString message = msg;
  QRegExp rx;
  switch(type) {
    case LT_ODS:
      kdDebug() << msg << endl;
      break;
    case LT_ERROR:
      //remove HTML tag
      rx.setPattern("\\[[^\\]]*\\]");
      message.remove(rx);

      switch(extInfo) {
        case E_ERROR:
        case E_CORE_ERROR:
        case E_PARSE:
        case E_COMPILE_ERROR:
        case E_USER_ERROR:
          manager()->debugMessage(DebuggerManager::ErrorMsg, message, filePath, line);
          manager()->debugError(message);
          break;

        case E_WARNING:
        case E_CORE_WARNING:
        case E_COMPILE_WARNING:
        case E_USER_WARNING:
          manager()->debugMessage(DebuggerManager::WarningMsg, message, filePath, line);
          break;
        case E_NOTICE:
        case E_USER_NOTICE:
        case E_STRICT:
          manager()->debugMessage(DebuggerManager::InfoMsg, message, filePath, line);
          break;
      }
      break;
    case LT_OUTPUT:
      m_output += msg;
      manager()->updateOutput(m_output);
      break;
    case LT_FATALERROR:
    break;
  }
}

void DebuggerDBG::checkDBGVersion(int major, int minor, const QString& desc)
{
  kdDebug() << desc << endl;

  if((major != DBG_API_MAJOR_VESION) ||
     (minor != DBG_API_MINOR_VESION)) {

    emit sigInternalError(QString("DBG version differs. Expecting %1.%2.").arg(
      QString::number(DBG_API_MAJOR_VESION), QString::number(DBG_API_MINOR_VESION)));
  }
}

/***************************** SLOTS *********************************/

void DebuggerDBG::slotInternalError(const QString& msg)
{
  emit sigInternalError(msg);
}

void DebuggerDBG::slotDBGStarted()
{

  m_net->requestOptions(SOF_SEND_LOGS
                       | SOF_SEND_ERRORS
                       | SOF_SEND_OUTPUT
                       | SOF_SEND_OUTPUT_DETAILED );

  m_isRunning = true;
  emit sigDebugStarted();
}

void DebuggerDBG::slotDBGClosed()
{
  //end of debug

  m_isRunning = false;
  m_output = QString::null;

  emit sigDebugEnded();
}

void DebuggerDBG::slotStepDone() {
  //assert m_currentExecutionPoint

  m_net->requestGlobalVariables();
  m_net->requestLocalVariables(m_currentExecutionPoint->id());
  requestWatches(m_currentExecutionPoint);
}

DBGConfiguration* DebuggerDBG::configuration()
{
  return m_configuration;
}

#include "debuggerdbg.moc"
