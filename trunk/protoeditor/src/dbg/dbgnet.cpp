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
#include "dbgnet.h"
#include "dbgresponsepack.h"
#include "dbgconnection.h"
#include "dbgreceiver.h"
#include "dbgrequestor.h"
#include "dbgnetdata.h"
#include "dbgstack.h"


DBGNet::DBGNet(DebuggerDBG* debugger, QObject *parent, const char *name)
    : QObject(parent, name), m_sessionId(0), m_headerFlags(0),
    m_debugger(debugger), m_con(0), m_receiver(0), m_requestor(0),
    m_dbgStack(0)//, m_dbgFileInfo(0)
{
  m_receiver    = new DBGReceiver(this);
  m_requestor   = new DBGRequestor();
  m_dbgStack    = new DBGStack();
  //m_dbgFileInfo = new DBGFileInfo();
}

DBGNet::~DBGNet()
{
  delete m_receiver;
  delete m_requestor;
  delete m_dbgStack;
  //delete m_dbgFileInfo;
}

bool DBGNet::startListener(int port)
{
  QHostAddress addr;
  addr.setAddress("localhost");

  if(m_con) delete m_con;

  m_con = new DBGConnection(addr, port);

  connect(m_con, SIGNAL(sigAccepted(QSocket*)), this, SLOT(slotIncomingConnection(QSocket*)));
  connect(m_con, SIGNAL(sigClosed()), this, SLOT(slotDBGClosed()));
  connect(m_con, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));

  return m_con->listening();
}

void DBGNet::stopListener()
{
  delete m_con;
  m_con = NULL;
}

void DBGNet::requestPage(const QString& host, const QString& filePath, int port, dbgint sessid)
{
  m_sessionId = sessid;
  m_requestor->makeHttpRequest(host, filePath, port, sessid);
}

void DBGNet::requestOptions(dbgint options)
{
  m_requestor->requestOptions(options);
}

void DBGNet::requestContinue()
{
  m_requestor->requestContinue();
  RequestCommonData();
}

void DBGNet::requestStop()
{
  m_requestor->requestStop();
}

void DBGNet::requestStepInto()
{
  m_requestor->requestStepInto();
  RequestCommonData();
}

void DBGNet::requestStepOver()
{
  m_requestor->requestStepOver();
  RequestCommonData();
}

void DBGNet::requestStepOut()
{
  m_requestor->requestStepOut();
  RequestCommonData();
}

void DBGNet::requestLocalVariables(dbgint scopeid)
{
  m_varScopeRequestList.push_back(scopeid);
  m_requestor->requestVariables(scopeid);
}

void DBGNet::requestGlobalVariables()
{
  //so we know if we are asking for a vars on global or local scope
  m_varScopeRequestList.push_back(GLOBAL_SCOPE_ID);
  m_requestor->requestVariables(GLOBAL_SCOPE_ID);
}

void DBGNet::requestWatch(const QString& expression, dbgint scopeid)
{
  m_varScopeRequestList.push_back(WATCH_SCOPE_ID);
  m_requestor->requestWatch(expression, scopeid);
}

void DBGNet::receivePack(DBGResponsePack* pack)
{
  if(!processHeader(pack->header())) {
    return;
  }

  DBGResponseTag* tag = NULL;
  pack->rewind();
  while((tag = pack->next()) != NULL) {
    tag->process(this, pack);
  }

  //updates data that need more than 1 run (like stack, that needs srctree to get the filenames)
  updatePendingData();
}

bool DBGNet::processHeader(DBGHeader* header)
{
  if(header->sync() != DBGSYNC) {
    error("Network sync error.");
    return false;
  }

  if(header->cmd() == DBGC_STARTUP) {
    //so lets start too
    m_requestor->addHeaderFlags(DBGF_STARTED);

    emit sigDBGStarted();
    RequestCommonData();
  }

  return true;
}

void DBGNet::processSessionId(dbgint sessiontype, const QString& sessionId)
{
  switch(sessiontype) {
  case DBG_JIT:
    m_sessionId = sessionId.toLong();
    break;
  case DBG_REQ:
    if(sessionId.toLong() != m_sessionId) {
      error("DBG requested a debug session with wrong session ID.");
      return;
    }
    break;
  case DBG_COMPAT:
  case DBG_EMB:
    //TODO: don't know about those
    break;
  }
}

void DBGNet::processStack(dbgint modno,const QString& descr,dbgint lineno, dbgint scopeid)
{
  m_dbgStack->add(modno, descr, lineno, scopeid);
}

void DBGNet::processDBGVersion(dbgint majorv, dbgint minorv, const QString& descr)
{
  /* I hope commenting this doesn't break anything :)

  if((majorv != DBG_API_MAJOR_VESION) || (minorv != DBG_API_MINOR_VESION)) {
    error(QString("DBG version differs. Expecting %1.%2.").arg(
      QString::number(DBG_API_MAJOR_VESION), QString::number(DBG_API_MINOR_VESION)));
  } */
}

void DBGNet::processSrcTree(dbgint modno, dbgint parentlineno, dbgint parentmodno, const QString& modname)
{
  m_dbgStack->setModulePath(modno, modname);
}

void DBGNet::processEval(const QString& result, const QString& str, const QString& error)
{
  dbgint scopeid = m_varScopeRequestList.first();
  m_varScopeRequestList.pop_front();
  if(scopeid == WATCH_SCOPE_ID) {
    m_debugger->updateWatch(result, str, error);
  } else {
    m_debugger->updateVar(result, str, error, scopeid);
  }
}

void DBGNet::processLog(dbgint type, const QString& log, dbgint modno, dbgint line, const QString& modname, dbgint extInfo)
{}

void DBGNet::processError(dbgint type, const QString& msg)
{}

void DBGNet::processBreakpoint(dbgint modno, dbgint lineno, const QString& modname,
                               dbgint state, dbgint istemp, dbgint hitcount, dbgint skiphits,
                               const QString& condition, dbgint bpno, dbgint isunderhit)
{}


void DBGNet::RequestCommonData()
{
  m_requestor->requestSrcTree();
  requestGlobalVariables();
}

void DBGNet::updatePendingData()
{
  if(m_dbgStack->ready()) {
    m_debugger->updateStack(m_dbgStack->debuggerStack());
  }
}

void DBGNet::slotIncomingConnection(QSocket* sock)
{
  m_receiver->setSocket(sock);
  m_requestor->setSocket(sock);
}

void DBGNet::slotDBGClosed()
{
  //stoping the debugger (not the session!!)

  emit sigDBGClosed();

  m_receiver->clear();
  m_requestor->clear();

  m_dbgStack->clear();
  m_varScopeRequestList.clear();
}

void DBGNet::slotError(const QString& msg)
{
  error(msg);
}

void DBGNet::error(const QString& msg)
{
  emit sigError(msg);
  m_con->closeClient();
}


#include "dbgnet.moc"
