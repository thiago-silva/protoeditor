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
#include <qsocketdevice.h>
#include <qsocketnotifier.h>

#include <kdebug.h>
#include <klocale.h>

DBGConnection::DBGConnection(QObject * parent, const char * name)
  : QObject(parent, name),
    m_currentClient(0), m_device(0), m_notifier(0), m_listening(false)

{
}

DBGConnection::~DBGConnection()
{
  close();
}

void DBGConnection::clearDevice()
{
  if(m_listening) {
    kdDebug() << "DBG: Stop listening on port " << m_device->port() << endl;
    m_device->close();
    m_listening = false;
  }

  delete m_device;
  m_device = NULL;
  delete m_notifier;
  m_notifier = NULL;
}

void DBGConnection::clearSocket()
{
  if(m_currentClient) {
    m_currentClient->close();
    delete m_currentClient;
    m_currentClient = NULL;
  }
}

void DBGConnection::slotIncomingConnection(int)
{
  int fd = m_device->accept();
  if(fd < 0) return;

  clearSocket();

  m_currentClient = new QSocket();
  m_currentClient->setSocket(fd);

  connect(m_currentClient, SIGNAL(connectionClosed()), this, SLOT(slotClosed()));
  connect(m_currentClient, SIGNAL(error(int)), this, SLOT(slotError(int)));

  emit sigAccepted(m_currentClient);
}

void DBGConnection::slotClosed()
{
  clearSocket();
  emit sigClientClosed();
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

bool DBGConnection::isListening()
{
  return m_listening;
}

void DBGConnection::closeClient()
{
  clearSocket();
}

void DBGConnection::close()
{
  closeClient();
  clearDevice();
}

bool DBGConnection::listenOn(int port)
{
  close();

  m_device = new QSocketDevice(QSocketDevice::Stream, QHostAddress().isIPv4Address()
            ? QSocketDevice::IPv4 : QSocketDevice::IPv6, 0);
  m_device->setAddressReusable( TRUE );

  if(m_device->bind(QHostAddress(), port)
      && m_device->listen(1))
  {
    m_listening = true;
    m_notifier = new QSocketNotifier(m_device->socket(), QSocketNotifier::Read,
            this, "accepting new connections" );

    connect( m_notifier, SIGNAL(activated(int)),
     this, SLOT(slotIncomingConnection(int)) );
  } else {
    m_listening = false;
    delete m_device;
    m_device = 0;
  }

  return isListening();
}




#include "dbgconnection.moc"
