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


#include "dbgheader.h"
#include "dbg_defs.h"
#include "dbgnethelper.h"

const int DBGHeader::SIZE = 16;

DBGHeader::DBGHeader()
 : DBGAbstractDataRequest(), m_sync(DBGSYNC), m_cmd(0), m_flags(0), m_bodysize(0)
{
}
DBGHeader::~DBGHeader() {
}

DBGHeader::DBGHeader(char* responseBuffer)
{
  m_sync     = DBGNetHelper::fromNetwork(responseBuffer,0);
  m_cmd      = DBGNetHelper::fromNetwork(responseBuffer,4);
  m_flags    = DBGNetHelper::fromNetwork(responseBuffer,8);
  m_bodysize = DBGNetHelper::fromNetwork(responseBuffer,12);
}

void DBGHeader::setSync(int sync)
{
    m_sync = sync;
}

void DBGHeader::setCmd(int cmd)
{
    m_cmd = cmd;
}

void DBGHeader::setFlags(int flags)
{
   m_flags = flags;
}

void DBGHeader::setBodysize(int bodysize)
{
    m_bodysize = bodysize;
}

int DBGHeader::sync()
{
    return m_sync;
}

int DBGHeader::cmd()
{
    return m_cmd;
}

int DBGHeader::flags()
{
    return m_flags;
}

int DBGHeader::bodysize()
{
    return m_bodysize;
}

char* DBGHeader::toArray()
{
  clearRawData();
  m_raw = new char[DBGHeader::SIZE];

  m_raw = DBGNetHelper::toNetwork(m_sync    , m_raw, 0);
  m_raw = DBGNetHelper::toNetwork(m_cmd     , m_raw, 4);
  m_raw = DBGNetHelper::toNetwork(m_flags   , m_raw, 8);
  m_raw = DBGNetHelper::toNetwork(m_bodysize, m_raw, 12);

  return m_raw;
}
