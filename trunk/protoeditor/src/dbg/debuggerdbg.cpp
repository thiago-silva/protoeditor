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
#include "debuggerconfigurations.h"
#include "debuggerstack.h"
#include "debuggerbreakpoint.h"
#include "dbgconfiguration.h"
#include "dbgstack.h"
#include "dbgreceiver.h"
#include "dbgrequestor.h"
#include "dbgconnection.h"
#include "dbg_defs.h"
#include "dbgtags.h"
#include "variableparser.h"
#include "dbgfileinfo.h"
#include "phpdefs.h"
#include <qhostaddress.h>
#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>

DebuggerDBG::DebuggerDBG(DebuggerManager* manager, DBGConfiguration* conf)
 : AbstractDebugger(manager), m_con(0), m_receiver(0), m_requestor(0),
   m_configuration(conf), m_dbgStack(0),m_dbgFileInfo(0), m_currentSessionId(0), m_initialized(false)
{
  setId(DBG);

  m_requestor   = new DBGRequestor();
  m_receiver    = new DBGReceiver(this);
  m_dbgStack    = new DBGStack();
  m_dbgFileInfo = new DBGFileInfo(conf);

  connect(m_requestor, SIGNAL(requestorError(const QString&)), this, SLOT(slotError(const QString&)));
  connect(m_receiver, SIGNAL(receiverError(const QString&)), this, SLOT(slotError(const QString&)));
}

DebuggerDBG::~DebuggerDBG()
{
  endSession();
  delete m_receiver;
  delete m_requestor;
}

void DebuggerDBG::startSession()
{
  if(isSessionActive()) return;

  QHostAddress addr;
  addr.setAddress("localhost");


  m_con = new DBGConnection(addr, m_configuration->listenPort());

  connect(m_con, SIGNAL(sigAccepted(QSocket*)), this, SLOT(slotIncomingConnection(QSocket*)));
  connect(m_con, SIGNAL(sigClosed()), this, SLOT(slotConnectionClosed()));
  connect(m_con, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));

  if(!m_con->listening())
  {
    emit internalError(i18n("Unable to listen on port: %1").arg(m_configuration->listenPort()));
  }
  else
  {
    setSessionActive(true);
    emit sigSessionStarted();
    kdDebug() << "DBG: listening on port " << m_configuration->listenPort() << endl;
  }
}

void DebuggerDBG::endSession()
{
  if(isSessionActive()) {
    stop();
    setSessionActive(false);
    clearConnection();
    m_currentSessionId = 0;

    m_dbgStack->clear();
    m_varScopeRequestList.clear();
    m_wathcesList.clear();

    emit sigSessionEnded();
    //** see slotConnectionClosed()
    //emit sigDebugEnded();
  }
}

void DebuggerDBG::clearConnection()
{
  delete m_con;
  m_con = NULL;
}

QString DebuggerDBG::name()
{
  return QString("DBG");
}

void DebuggerDBG::loadConfiguration(DebuggerConfigurations* conf) {
  m_configuration = conf->dbgConfiguration();
  m_dbgFileInfo->setConfiguration(m_configuration);
}

void DebuggerDBG::run(QString filepath)
{
  if(!isSessionActive()) return;

  if(!isRunning()) {
    m_currentSessionId = kapp->random();

    m_requestor->makeHttpRequest(
          m_configuration->host(),
          filepath.remove(m_configuration->localBaseDir()),
          m_configuration->listenPort(),
          m_currentSessionId);
  } else {
    m_requestor->requestContinue();
  }

  //** see slotServerAccepted()
  //setRunning(true);
  //emit sigDebugStarted();
}

void DebuggerDBG::stop()
{
  if(isRunning()) {
    m_requestor->requestStop();
  }
}

void DebuggerDBG::stepOver()
{
  if(isRunning()) {
    m_requestor->requestStepOver();
    requestData();
  }
}

void DebuggerDBG::stepInto()
{
  if(isRunning()) {
    m_requestor->requestStepInto();
    m_requestor->requestSrcTree();
    requestData();
  }
}

void DebuggerDBG::stepOut()
{
  if(isRunning()) {
    m_requestor->requestStepOut();
    requestData();
  }
}

void DebuggerDBG::addWatch(QString expression, DebuggerExecutionLine* stackContext)
{
  m_wathcesList.push_back(expression);

  if(isRunning()) {
    int scope_id = (stackContext)?stackContext->id():GLOBAL_SCOPE_ID;
    m_requestor->requestWatch(expression, scope_id);
  } else {
    PHPVariable* var = new PHPVariable(expression);
    var->setValue(new PHPScalarValue(var));
    var->value()->setScalar(true);
    emit sigWatchChanged(var);
  }
}

void DebuggerDBG::removeWatch(QString expression) {
  QValueList<QString>::iterator it = m_wathcesList.find(expression);

  if(it != m_wathcesList.end()) {
     m_wathcesList.remove(it);
  }
}

void DebuggerDBG::requestWatches(DebuggerExecutionLine* stackContext)
{
  if(isRunning()) {
    QValueList<QString>::iterator it;
    for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it) {
      m_requestor->requestWatch(*it, stackContext->id());
    }
  }
}

void DebuggerDBG::addBreakpoints(QPtrList<DebuggerBreakpoint> list) {
  if(isRunning()) {
    int modno;
    QString localfile;

    DebuggerBreakpoint* bp;
    for(bp = list.first(); bp; bp = list.next()) {
      modno = m_dbgFileInfo->moduleNumber(bp->filePath());
      localfile = bp->filePath();
      bp->setFilePath(
        localfile.replace(m_configuration->localBaseDir(),m_configuration->serverBaseDir()));
      m_requestor->requestBreakpoint(
        modno, bp);
    }
  }
}

void DebuggerDBG::addBreakpoint(DebuggerBreakpoint* bp) {
  if(!bp) return;

  if(isRunning()) {
    int modno = m_dbgFileInfo->moduleNumber(bp->filePath());
    QString localfile = bp->filePath();
    bp->setFilePath(
      localfile.replace(m_configuration->localBaseDir(),m_configuration->serverBaseDir()));

    m_requestor->requestBreakpoint(modno, bp);
    m_requestor->requestBreakpointList(0);
  }
}

void DebuggerDBG::changeBreakpoint(DebuggerBreakpoint* bp) {

  if(isRunning()) {
    m_requestor->requestBreakpoint(
      m_dbgFileInfo->moduleNumber(bp->filePath()), bp);
  }
}

void DebuggerDBG::removeBreakpoint(DebuggerBreakpoint* bp) {
  if(!bp) return;

  //m_bpList.remove(bp);
  if(isRunning()) {
    m_requestor->requestBreakpointRemoval(bp);
  }
}

void DebuggerDBG::requestLocalVariables(DebuggerExecutionLine* stackContext) {
  if(isRunning()) {
    if(stackContext) {
      m_varScopeRequestList.push_back(false);
      m_requestor->requestVariables(stackContext->id());
    }
  }
}

void DebuggerDBG::modifyVariable(Variable* v, DebuggerExecutionLine* stackContext) {
  if(isRunning()) {
    QString name  =  v->compositeName();
    QString value = v->value()->toString();

    if(value.isEmpty()) value = "null";

    m_requestor->requestWatch(name + "=" + value, stackContext->id());

    //reload variables
    requestGlobalVariables();

    requestLocalVariables(stackContext);

    //m_requestor->requestVariables(GLOBAL_SCOPE_ID);
    //m_requestor->requestVariables(stackContext->id());
  }
}

void DebuggerDBG::requestData() {
  requestGlobalVariables();
  //requestBreakpoints();
}

void DebuggerDBG::requestGlobalVariables()
{
  if(isRunning()) {
    m_varScopeRequestList.push_back(true);
    m_requestor->requestVariables(GLOBAL_SCOPE_ID);

  }
}

void DebuggerDBG::updateSessionInfo(DBGResponseTagSid* sid, DBGTagRawdata* raw)
{
  if(sid->sesstype() == DBG_JIT) {
    setRunning(true); //starting JIT session
  } else {
    QString id = raw->data();
    if((sid->sesstype() == DBG_COMPAT) && (m_currentSessionId != id.toLong())) {
      emit internalError(i18n("session ID is diferent"));
      return;
    }
  }
}

void DebuggerDBG::updateDebuggerVersion(DBGResponseTagVersion* version, DBGTagRawdata* raw)
{
  if((version->majorVersion() != 2) || (version->minorVersion() != 17)) {
    emit internalError((i18n("unsupported DBG version")));
    endSession();
  }
}

void DebuggerDBG::updateBreakpoints(DBGResponsePack::BpTagMap_t bpMap) {

  DBGResponsePack::BpTagMap_t::Iterator it;
  for(it = bpMap.begin(); it != bpMap.end(); ++it ) {
    sendBreakpoint(it.key(), it.data().first, it.data().second);
  }
}

void DebuggerDBG::sendBreakpoint(DBGTagBreakpoint* bpTag, DBGTagRawdata* fileraw, DBGTagRawdata* condraw)
{
  QString filePath, condition;
  if(fileraw) {
    filePath = fileraw->data();
  }

  if(condraw) {
    condition = condraw->data();
  }

  int state;
  switch(bpTag->state()) {
    case BPS_ENABLED:
      state = DebuggerBreakpoint::ENABLED;
      break;
    case BPS_DISABLED:
      state = DebuggerBreakpoint::DISABLED;
      break;
    case BPS_DELETED:
      if(bpTag->bpNo()) {
          //BreakpointListView already deleted the breakpoint
          //so we don't need to acknowledge this
          return;
      } else {
        //when DBG doesn't process the breakpoint requested
        //because of the file it is on (ie. php didn't read it yet)
        //We have to tell the views that its unresolved
        state = DebuggerBreakpoint::UNRESOLVED;
      }
      break;
    default:
      state = DebuggerBreakpoint::UNRESOLVED;
      break;
  }

  DebuggerBreakpoint* bp = new
    DebuggerBreakpoint(bpTag->bpNo()
                     , filePath
                     , bpTag->lineNo()
                     , state
                     , condition
                     ,  bpTag->hitCount()
                     , bpTag->skipHits());

  emit sigBreakpointChanged(bp);
}

void DebuggerDBG::updateVariables(DBGResponseTagEval* eval, DBGTagRawdata* result , DBGTagRawdata* istr, DBGTagRawdata* error)
{
  if(!(result || istr || error)) {
    internalError(QString("Error requesting variables"));
    return;
  }

  if(!istr) {
    bool isGlobalContext = m_varScopeRequestList.first();
    m_varScopeRequestList.pop_front();

    VariableParser p(result->data());

    VariablesList_t* array = p.parseAnonymousArray();

    //backup the varlist so we can use to parse watches
    //if(isGlobalContext) {
    //  m_indexedVarList = p.indexedVarList();
    //}

    emit sigVariablesChanged(array,isGlobalContext);
  } else {
    //this came from watch request...
    PHPVariable* var;
    if(result) {
      VariableParser p(result->data());
      //p.setIndexedVarList(m_indexedVarList);

      var = p.parseVariable();
      var->setName(istr->data());
    } else {
      var = new PHPVariable(istr->data());
      PHPScalarValue* value = new PHPScalarValue(var);
      var->setValue(value);
    }

    emit sigWatchChanged(var);
  }

  if(error) {
    //emit sigDebugError(error->data());
  }
}

void DebuggerDBG::updateStack()
{
  if(!m_initialized) {
    m_initialized = true;
    //DebuggerBreakpoint* b = new DebuggerBreakpoint(0, "/usr/local/apache/htdocs/texto.php",8);
    //m_requestor->requestBreakpoint(
    //  m_dbgFileInfo->moduleNumber("/usr/local/apache/htdocs/texto.php"), b);
    //emit sigDebugStarted();
  }
  emit sigStackChanged(m_dbgStack->debuggerStack(m_dbgFileInfo));
}

void DebuggerDBG::showLog(DBGResponseTagLog* log, DBGTagRawdata* rawmod, DBGTagRawdata* rawlog)
{
  switch(log->type()) {
    case 3:
      emit sigDebugOutput(rawlog->data());
      break;
    case 2:
      switch(log->type()) {
        case E_ERROR:
        case E_CORE_ERROR:
        case E_COMPILE_ERROR:
        case E_USER_ERROR:
        case E_PARSE:
          emit sigDebugMessage(DebuggerManager::ErrorMsg, rawlog->data(), log->lineNo(), rawmod->data());
          break;
        case E_WARNING:
        case E_CORE_WARNING:
        case E_COMPILE_WARNING:
        case E_USER_WARNING:
          emit sigDebugMessage(DebuggerManager::WarningMsg, rawlog->data(), log->lineNo(), rawmod->data());
          break;
        case E_NOTICE:
        case E_USER_NOTICE:
        case E_STRICT:
          emit sigDebugMessage(DebuggerManager::InfoMsg, rawlog->data(), log->lineNo(), rawmod->data());
      }
    break;
  }

  QString message;
  QString module;
  if(rawmod) {
    message = rawmod->data();
  }

  if(rawlog) {
    module = rawlog->data();
  }

  kdDebug() << "DBG LOG: module=" <<
    module << ", line=" << log->lineNo() <<  ", message=\"" << message << "\", type=" <<
    log->type() << ", ext=" << log->extInfo() << endl;

}

void DebuggerDBG::showError(DBGResponseTagError* error, DBGTagRawdata* raw)
{
  //TODO: make sigDebugError(title, message)
  //the title is the type of  error->type()

  emit sigDebugError(raw->data());
}

void DebuggerDBG::receivePack(DBGResponsePack* pack)
{
  //TODO: create a Visitor class and add to the tags a visitMe(), to get rid of this function

  if(pack->has(FRAME_SID))
  {
    DBGResponseTagSid* sid;
    sid = dynamic_cast<DBGResponseTagSid*>(pack->retrieve(FRAME_SID));
    updateSessionInfo(sid, pack->retrieveRawdata(sid->isid()));
  }

  if(pack->has(FRAME_VER))
  {
    DBGResponseTagVersion* version;
    version = dynamic_cast<DBGResponseTagVersion*>(pack->retrieve(FRAME_VER));
    updateDebuggerVersion(version, pack->retrieveRawdata(version->idescription()));
  }

  if(pack->has(FRAME_BPS))
  {
    DBGTagBreakpoint* bps;
    bps = dynamic_cast<DBGTagBreakpoint*>(pack->retrieve(FRAME_BPS));
    updateBreakpoints(pack->retrieveBpTagMap());
  }

  if(pack->has(FRAME_EVAL))
  {
    DBGResponseTagEval* eval;
    eval = dynamic_cast<DBGResponseTagEval*>(pack->retrieve(FRAME_EVAL));
    updateVariables(eval, pack->retrieveRawdata(eval->iresult())
                        , pack->retrieveRawdata(eval->istr())
                        , pack->retrieveRawdata(eval->ierror()));
  }

  if(pack->has(FRAME_STACK))
  {
    //do not update the stack yet because we still don't have file path informations
    //so, we request the srctree (wich give's us the paths with their IDs)
    m_dbgStack->clear();
    m_dbgStack->setStackTagList(pack->retrieveStackTagList());

    m_requestor->requestSrcTree();
  }

  if(pack->has(FRAME_SRC_TREE))
  {
    //updates the modules id (used by stack, breakpooints, etc)
    //to build correctly the stack previously loaded.
    //then, updates the stack.
    m_dbgFileInfo->loadFilePathInformation(pack->retrieveTreeTagList());
    updateStack();
  }

  if(pack->has(FRAME_LOG)) {
    DBGResponseTagLog* log;
    log = dynamic_cast<DBGResponseTagLog*>(pack->retrieve(FRAME_LOG));
    DBGTagRawdata* rawmod = pack->retrieveRawdata(log->imodName());
    DBGTagRawdata* rawlog = pack->retrieveRawdata(log->ilog());
    showLog(log, rawmod, rawlog);
  }

  if(pack->has(FRAME_ERROR)) {
    DBGResponseTagError* error;
    error = dynamic_cast<DBGResponseTagError*>(pack->retrieve(FRAME_ERROR));
    DBGTagRawdata* raw = pack->retrieveRawdata(error->imessage());
    showError(error, raw);
  }

  if(pack->has(FRAME_BPL)) {
    int a;
    a = 2;
  }
}

void DebuggerDBG::slotError(const QString& errorMsg)
{
  emit internalError(errorMsg);
}

void DebuggerDBG::slotIncomingConnection(QSocket* sock)
{
  m_receiver->setSocket(sock);
  m_requestor->setSocket(sock);

  setRunning(true);
  emit sigDebugStarted();
  requestData();

  //testing !!
  //m_requestor->requestOptions(SOF_BREAKONLOAD | SOF_BREAKONFINISH | SOF_SEND_OUTPUT_DETAILED);
  //

  //step into the first line
  //stepInto();
}

void DebuggerDBG::slotConnectionClosed()
{
  m_receiver->setSocket(NULL);
  m_requestor->setSocket(NULL);

  stopDebugger();
}

void DebuggerDBG::stopDebugger() {
  //stoping the debugger (not the session!!)

  m_initialized = false;
  setRunning(false);
  m_dbgStack->clear();
  m_dbgFileInfo->clear();
  m_currentSessionId = 0;
  emit sigDebugEnded();
}

void DebuggerDBG::internalError(QString error) {
  emit sigInternalError(error);
  endSession();
}

#include "debuggerdbg.moc"
