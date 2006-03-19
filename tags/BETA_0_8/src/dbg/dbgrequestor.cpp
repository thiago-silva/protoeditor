/***************************************************************************
 *   Copyright (C) 2004-2005 by Thiago Silva                                    *
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
#include "dbgnetdata.h"
#include "dbgrequestpack.h"
#include "dbgrequestpackbuilder.h"

#include <qsocket.h>
#include <kurl.h>

DBGRequestor::DBGRequestor()
    : QObject(), m_socket(NULL)/*, m_http(0)*/, m_headerFlags(0) /*, m_terminating(false)*/
{
}

DBGRequestor::~DBGRequestor()
{

}

void DBGRequestor::requestContinue()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_CONTINUE);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestStop()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STOP);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestStepInto()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPINTO);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestStepOver()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPOVER);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestStepOut()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPOUT);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestWatch(const QString& expression,int scope_id)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildWatch(expression, scope_id);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestBreakpoint(int bpno, int modno, const QString& remoteFilePath, int line, const QString& condition, int status, int skiphits, bool istemp)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildBreakpoint(
      bpno, modno, remoteFilePath, line, condition, status, skiphits, istemp);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestBreakpointRemoval(int bpid)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildDeletedBreakpoint(bpid);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestVariables(int scope_id)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildVars(scope_id);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestSrcTree()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildSrcTree();
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestSrcLinesInfo(int modno)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildSrcLinesInfo(modno);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestSrcCtxInfo(int modno)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildSrcCtxInfo(modno);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}
  
void DBGRequestor::requestOptions(int op)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildOptions(op);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestProfileData(int modno)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildProfile(modno);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestProfileFreqData(int testLoops)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildProfileC(testLoops);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::addHeaderFlags(dbgint flags)
{
  m_headerFlags |= flags;
}

void DBGRequestor::setSocket(QSocket* socket)
{
  m_socket = socket;
}

void DBGRequestor::clear()
{
  m_socket = NULL;
}


#include "dbgrequestor.moc"
