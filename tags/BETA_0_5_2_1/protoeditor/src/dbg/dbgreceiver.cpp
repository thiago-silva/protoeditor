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
#include "debuggerdbg.h"
#include "dbg_defs.h"
#include "dbgresponsepack.h"
#include "dbgheader.h"
#include "dbgframe.h"
#include "dbgtags.h"


#include <qsocket.h>
#include <kdebug.h>
#include <klocale.h>


DBGReceiver::DBGReceiver(DebuggerDBG* debugger, QObject *parent, const char *name)
 : QObject(parent, name), m_socket(0), m_debugger(debugger)
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

void DBGReceiver::syncBuffer()
{
  if(!m_socket || (m_socket->socket() == -1))  return;

  int size = m_socket->size();
  char* data = new char[size];
  m_socket->readBlock(data, size);
  delete data;
}

void DBGReceiver::slotReadBuffer()
{
  //this shouldn't happen...
  if(!m_socket || (m_socket->socket() == -1))  return;

  DBGResponsePack* pack = NULL;
  DBGHeader* header     = NULL;
  DBGFrame* frame       = NULL;
  DBGBaseTag* tag       = NULL;
  char* buffer          = NULL;

  while(m_socket->size()) {
    if((header = readHeader()) == NULL) {
      break;
    }

    if(header->sync() != DBGSYNC) {
      syncBuffer();
      break; //out of sync
    }

    if(header->bodysize() <= 0) {
      continue; //no body here
    }

    pack = NULL;
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

      if(!pack) pack = new DBGResponsePack();

      pack->addTag(tag);

      total += DBGFrame::SIZE + frame->datasize();

      delete frame;
      frame = NULL;
      delete [] buffer;
      buffer = NULL;
    }

    if(pack)
    {
      m_debugger->receivePack(pack);
    }

    delete header;
    header = NULL;
    delete pack;
    pack = NULL;
  }
}


DBGHeader*  DBGReceiver::readHeader()
{
  char data[16];
  long ret;
  if((ret = m_socket->readBlock(data, 16)) != 16) {
    return NULL;
  } else {
    return new DBGHeader(data);
  }

}
DBGFrame* DBGReceiver::readFrame()
{
  char data[8];
  int ret;
  if((ret = m_socket->readBlock(data, 8)) != 8) {
    return NULL;
  } else {
    return new DBGFrame(data);
  }
}
DBGBaseTag* DBGReceiver::buildTag(int frameName, char* buffer)
{
  DBGBaseTag* tag = NULL;
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
      tag = new DBGResponseTagSrcLinesInfo(buffer);
      break;
    case FRAME_LOG:
      tag = new DBGResponseTagLog(buffer);
      break;
    case FRAME_ERROR:
      tag = new DBGResponseTagError(buffer);
      break;
    default:
      kdDebug(24000) << "UNKNOW FRAME: " << QString::number(frameName) << endl;
      break;
  }
  return tag;
}
char* DBGReceiver::readData(long bytesToRead)
{
  /* Note: make sure there are the bytesToRead available
     before attempting to read.
  */

  char* data = new char[bytesToRead];
  long read = 0;
  long available = m_socket->bytesAvailable();

  while(available < bytesToRead) {
    available = m_socket->waitForMore(-1);
  }

  while(bytesToRead) {
    if((read = m_socket->readBlock(data, bytesToRead)) == bytesToRead) {
      return data;
    }

    bytesToRead -= read;

    available = m_socket->waitForMore(-1);

    long newread = m_socket->readBlock(&data[read], bytesToRead);

    bytesToRead -= newread;
  }

  return data;

/*
  if((ret = m_socket->readBlock(data, len)) != len) {
    while((available = m_socket->bytesAvailable()) == 0) {
      available = m_socket->waitForMore(-1);
      int ret2;
      if((ret2 = m_socket->readBlock(&data[len], available)) == 0) {
        return NULL;
      }
      ret += ret2;
      while(
    }

    return NULL;
  } else {
    return data;
  }
*/
}


#include "dbgreceiver.moc"
