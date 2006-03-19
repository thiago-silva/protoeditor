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

#include "dbgresponsepack.h"
#include "dbgnetdata.h"
#include "dbg_defs.h"
#include <qsocket.h>

DBGResponsePack::DBGResponsePack()
    : m_index(0), m_header(0)
{}

DBGResponsePack::~DBGResponsePack()
{
  for(DBGResponseTag *tag = m_tags.first(); tag ; tag = m_tags.next() ) {
    delete tag;
  }

  for(DBGResponseTag *rawtag = m_rawTags.first(); rawtag ; rawtag = m_rawTags.next() ) {
    delete rawtag;
  }

  delete m_header;
}

void DBGResponsePack::setHeader(DBGHeader* header)
{
  m_header = header;
}

DBGHeader* DBGResponsePack::header()
{
  return m_header;
}


void DBGResponsePack::addTag(DBGResponseTag* tag)
{
  if(tag->name() == FRAME_RAWDATA) {
    m_rawTags.append(tag);
  } else {
    m_tags.append(tag);
  }
}

/*
DBGBaseTag* DBGResponsePack::retrieve(int framename)
{
  for(DBGBaseTag *tag = m_baseTags.first(); tag ; tag = m_baseTags.next() ) {
    if(tag->name() == framename) {
      return tag;
    }
  }
  return NULL;
}
*/


DBGTagRawdata* DBGResponsePack::retrieveRawdata(int id)
{
  for(DBGResponseTag *tag = m_rawTags.first(); tag ; tag = m_rawTags.next() ) {
    DBGTagRawdata* raw = dynamic_cast<DBGTagRawdata*>(tag);
    if(raw->id() == id) {
      return raw;
    }
  }
  return NULL;
}

void DBGResponsePack::rewind()
{
  m_index = 0;
}

DBGResponseTag* DBGResponsePack::next()
{
  return m_tags.at(m_index++);
}
