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


#include "dbgconnection.h"
#include <qsocket.h>
#include <klocale.h>

DBGConnection::DBGConnection(const QHostAddress& host, int port, QObject * parent, const char * name)
    : QServerSocket(host, port, 1, parent, name), m_currentSocket(0)
{
  m_listening = ok();
}

DBGConnection::~DBGConnection()
{
  clearSocket();
}

void DBGConnection::clearSocket()
{
  if(m_currentSocket) {
    delete m_currentSocket;
    m_currentSocket = NULL;
    emit sigClosed();
  }
}

void DBGConnection::newConnection(int socket)
{
  clearSocket();


  m_currentSocket = new QSocket();
  m_currentSocket->setSocket(socket);
  //m_currentSocket->socketDevice()->setBlocking(true);

  connect(m_currentSocket, SIGNAL(connectionClosed()), this, SLOT(slotClosed()));
  connect(m_currentSocket, SIGNAL(error(int)), this, SLOT(slotError(int)));

  emit sigAccepted(m_currentSocket);
}

void DBGConnection::slotClosed()
{
  clearSocket();
}

void DBGConnection::slotError(int error)
{
  switch(error) {
  case QSocket::ErrConnectionRefused:
    emit sigError(QString("Connection refused"));
    break;
  case QSocket::ErrHostNotFound:
    emit sigError(QString("Host not found"));
    break;
  case QSocket::ErrSocketRead:
    emit sigError(QString("Error reading network data"));
    break;
  }
}

bool DBGConnection::listening()
{
  return m_listening;
}

void DBGConnection::closeClient()
{
  clearSocket();
}


#include "dbgconnection.moc"
