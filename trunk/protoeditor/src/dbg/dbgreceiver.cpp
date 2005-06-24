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


#include "dbgreceiver.h"
#include "dbgnet.h"
#include "dbg_defs.h"
#include "dbgresponsepack.h"
#include "dbgnetdata.h"


#include <qsocket.h>
#include <kdebug.h>
#include <klocale.h>


DBGReceiver::DBGReceiver(DBGNet* net, const char *name)
 : QObject(net, name), m_socket(0), m_net(net)
{
}

DBGReceiver::~DBGReceiver()
{
}

void DBGReceiver::setSocket(QSocket* socket)
{
  m_socket = socket;
  if(m_socket) {
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadBuffer()));
  }
}

void DBGReceiver::clear() {
  m_socket = NULL;
}

void DBGReceiver::syncBuffer()
{
  if(!m_socket || (m_socket->socket() == -1))  return;

  int size = m_socket->size();
  char* data = new char[size];
  m_socket->readBlock(data, size);
  delete data;
}

void DBGReceiver::readBuffer() {

  DBGResponsePack* pack   = NULL;
  DBGHeader*       header = NULL;
  DBGFrame*        frame  = NULL;
  DBGResponseTag*  tag    = NULL;
  char* buffer            = NULL;

  while(m_socket && m_socket->size()) {

    pack = new DBGResponsePack();

    if((header = readHeader()) == NULL) {
      break;
    }

    if(header->sync() != DBGSYNC) {
      syncBuffer();
      break; //out of sync
    }

    pack->setHeader(header);

    /*
    if(header->bodysize() <= 0) {
      break; //no body here
    }
    */

    int total = 0;

    while(total < header->bodysize()) {
      if((frame = readFrame()) == NULL) {
        syncBuffer();
        break;
      }

      if((buffer = readData(frame->datasize())) == NULL) {
        syncBuffer();
        break;
      }

      if((tag = buildTag(frame->name(), buffer)) == NULL) {
        syncBuffer();
        break;
      }

      pack->addTag(tag);

      total += DBGFrame::SIZE + frame->datasize();

      delete frame;
      frame = NULL;
      delete [] buffer;
      buffer = NULL;
    }

    if(pack) {
      m_net->receivePack(pack);
    }

    delete pack;
    pack = NULL;
  }
}

void DBGReceiver::slotReadBuffer()
{
  readBuffer();
}



DBGHeader*  DBGReceiver::readHeader()
{
  if(!m_socket) return NULL;

  char data[DBGHeader::SIZE];
  long ret;
  if((ret = m_socket->readBlock(data, DBGHeader::SIZE)) != DBGHeader::SIZE) {
    return NULL;
  } else {
    return new DBGHeader(data);
  }

}
DBGFrame* DBGReceiver::readFrame()
{
  if(!m_socket) return NULL;

  char data[DBGFrame::SIZE];
  int ret;
  if((ret = m_socket->readBlock(data, DBGFrame::SIZE)) != DBGFrame::SIZE) {
    return NULL;
  } else {
    return new DBGFrame(data);
  }
}
DBGResponseTag* DBGReceiver::buildTag(int frameName, char* buffer)
{
  DBGResponseTag* tag = NULL;
  switch(frameName) {
    case FRAME_SID:
      tag = new DBGResponseTagSid(buffer);
      break;
    case FRAME_RAWDATA:
      tag = new DBGTagRawdata(buffer);
      break;
    case FRAME_VER:
      tag = new DBGResponseTagVersion(buffer);
      break;
    case FRAME_STACK:
      tag = new DBGResponseTagStack(buffer);
      break;
    case FRAME_BPL:
      tag = new DBGTagBreakpoint(buffer);
      break;
    case FRAME_EVAL:
      tag = new DBGResponseTagEval(buffer);
      break;
    case FRAME_SRC_TREE:
      tag = new DBGResponseTagSrcTree(buffer);
      break;
    case FRAME_LOG:
      tag = new DBGResponseTagLog(buffer);
      break;
    case FRAME_ERROR:
      tag = new DBGResponseTagError(buffer);
      break;
    case FRAME_PROF:
      tag = new DBGResponseTagProf(buffer);
      break;
    case FRAME_PROF_C:
      tag = new DBGResponseTagProfC(buffer);
      break;
    case FRAME_SRCLINESINFO:
      tag = new DBGResponseTagSrcLinesInfo(buffer);
      break;
    case FRAME_SRCCTXINFO:
      tag = new DBGResponseTagSrcCtxInfo(buffer);
      break;
    default:
      emit sigError("Error receiving network data.");
      break;
  }
  return tag;
}
char* DBGReceiver::readData(long bytesToRead)
{
  if(!m_socket) return NULL;

  /* Note: make sure there are the bytesToRead available
     before attempting to read.
  */

  char* data = new char[bytesToRead];
  long read = 0;
  long last = 0;
  long available = m_socket->bytesAvailable();

  while(available < bytesToRead) {
    available = m_socket->waitForMore(-1);
  }

  while(bytesToRead) {
    if((read = m_socket->readBlock(data, bytesToRead)) == bytesToRead) {
      return data;
    }

    //I've never seen this code being executed....
    bytesToRead -= read;
    last += read;
    
    available = m_socket->waitForMore(-1);
    bytesToRead -= m_socket->readBlock(&data[last], available);
  }

  return data;
}

#include "dbgreceiver.moc"
