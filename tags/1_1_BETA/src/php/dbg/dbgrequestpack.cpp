/***************************************************************************
 *   Copyright (C) 2004-2006 by Thiago Silva                               *
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

#include "dbgrequestpack.h"

#include <qsocket.h>
#include "dbg_defs.h"
#include "dbgnetdata.h"
#include <kdebug.h>

/*
DBGRequestPack::DBGRequestPack()
{
  m_header = new DBGHeader();
}
*/

DBGRequestPack::DBGRequestPack(int cmd)
{
  m_header = new DBGHeader();
  m_header->setCmd(cmd);
}

DBGRequestPack::~DBGRequestPack()
{
  DBGFrame* frame;
  DBGRequestTag* tag;
  List_t::iterator it;
  for(it = m_list.begin(); it != m_list.end(); ++it ) {
    frame = (*it).first;
    tag   = (*it).second;
    delete frame;
    if(tag) delete tag;
  }

  m_list.clear();

  delete m_header;
  m_header = NULL;
}

void DBGRequestPack::addInfo(DBGFrame* frame, DBGRequestTag* dbgTag)
{
  m_list.push_back(FrameReqPair_t(frame, dbgTag));
}

void DBGRequestPack::send(QSocket* socket)
{
  m_header->setBodysize(calcBodySize());

  socket->writeBlock(m_header->toArray(), DBGHeader::SIZE);

  DBGFrame* frame;
  DBGRequestTag* tag;

  List_t::iterator it;
  for(it = m_list.begin(); it != m_list.end(); ++it ) {
    frame = (*it).first;
    tag   = (*it).second;

    socket->writeBlock(frame->toArray(), DBGFrame::SIZE);

    if(tag)
    {
      socket->writeBlock(tag->toArray(), tag->tagSize());
    }
  }
}

int DBGRequestPack::calcBodySize() {

  DBGRequestTag* tag;
  int total = 0;

  List_t::iterator it;
  for(it = m_list.begin(); it != m_list.end(); ++it ) {
    total += DBGFrame::SIZE;

    tag = (*it).second;
    if(tag) {
      total += tag->tagSize();
    }
  }
  return total;
}

DBGHeader* DBGRequestPack::header() {
  return m_header;
}

/*
  int bodysize = 0;
  map_t::const_iterator end = m_map.end();
  for(map_t::const_iterator it = m_map.begin(); it != end; ++it)
  {
    bodysize += DBGFrame::SIZE;
    if(it->second)
    {
      bodysize += it->second->tagSize();
    }
  }

  m_header->setBodysize(bodysize);


  socket->writeBlock(m_header->toArray(), DBGHeader::SIZE);

  char* data;
  for(map_t::const_iterator it = m_map.begin(); it != end; ++it)
  {
    data = it->first->toArray();

    socket->writeBlock(data, DBGFrame::SIZE);
    delete data;

    if(it->second)
    {
      data = it->second->toArray();
      socket->writeBlock(data, it->second->tagSize());
      delete data;
    }
  }
*/

