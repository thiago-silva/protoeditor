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


#include "dbgrequestor.h"

#include "debuggerbreakpoint.h"
#include "dbg_defs.h"
#include "dbgrequestpack.h"
#include "dbgrequestpackbuilder.h"

#include <qsocket.h>
#include <qhttp.h>
#include <klocale.h>
#include <kdebug.h>

DBGRequestor::DBGRequestor()
  : QObject(), m_socket(NULL), m_terminating(false)
{
  m_http = new QHttp;;
  connect(m_http, SIGNAL(done(bool)), this, SLOT(slotHttpDone(bool)));
}

DBGRequestor::~DBGRequestor()
{
  m_terminating = true;
  delete m_http;
  //deletePacks();
}

void DBGRequestor::requestContinue()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_CONTINUE);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestStop()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STOP);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestStepInto()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPINTO);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestStepOver()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPOVER);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestStepOut()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPOUT);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestWatch(const QString& expression,int scope_id)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildWatch(expression, scope_id);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestBreakpoint(int modno, DebuggerBreakpoint* bp) {
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildBreakpoint(modno, bp);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestBreakpointList(int bpno) {
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildBreakpointList(bpno);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestBreakpointRemoval(DebuggerBreakpoint* bp) {
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildDeletedBreakpoint(bp->id());
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestVariables(int scope_id)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildVars(scope_id);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestSrcTree()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildSrcTree();
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::requestOptions(int op) {
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildOptions(op);
  requestPack->send(m_socket);
  clearPack(requestPack);
}

void DBGRequestor::makeHttpRequest(const QString& host, const QString& path, int listenPort, int sessionId)
{
  //trigger the DBG server sending a http request to the web server

  QString reqUrl = path + "?DBGSESSID="
                       + QString::number(sessionId)
                       + "@clienthost:"
                       + QString::number(listenPort);

  kdDebug() << "Requesting \"" << reqUrl << "\" from " + host << endl;

  m_http->setHost(host);
  m_http->get(reqUrl);
}

void DBGRequestor::setSocket(QSocket* socket)
{
  m_socket = socket;
  if(m_socket) {
    connect(m_socket, SIGNAL(connectionClosed()), this, SLOT(slotClosed()));
  }
}

void DBGRequestor::slotHttpDone(bool error) {
  if(error) {
    emit requestorError(i18n("HTTP Conection error: " + m_http->errorString()));
  }
}

/*
void DBGRequestor::flushData() {
  while(m_socket->bytesToWrite()) {
    m_socket->flush();
  }
}
*/

void DBGRequestor::slotClosed() {
  deletePacks();
}

void DBGRequestor::deletePacks() {
  DBGRequestPack* p;
  for(p = m_deleteList.first(); p; p = m_deleteList.next()) {
    delete p;
  }

  m_deleteList.clear();
}

void DBGRequestor::clearPack(DBGRequestPack* requestPack) {
  /* FIXME: deleting requestPack is causing the app to crash when
   * it was builded by this->requestWatch()
   */

  //delete requestPack;

  m_deleteList.append(requestPack);
}

#include "dbgrequestor.moc"
