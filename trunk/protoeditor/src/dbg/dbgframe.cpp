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


#include "dbgframe.h"
#include "dbgnethelper.h"

const int DBGFrame::SIZE = 8;

DBGFrame::DBGFrame()
    :  DBGAbstractDataRequest(), m_name(0), m_datasize(0)
{
}
DBGFrame::DBGFrame(int name, int datasize)
{
  m_name = name;
  m_datasize = datasize;
}

DBGFrame::DBGFrame(char* responseBuffer)
{
  m_name     = DBGNetHelper::fromNetwork(responseBuffer, 0);
  m_datasize = DBGNetHelper::fromNetwork(responseBuffer, 4);
}

DBGFrame::~DBGFrame() {
}

void DBGFrame::setName(int name)
{
    m_name = name;
}

void DBGFrame::setDatasize(int datasize)
{
    m_datasize = datasize;
}

int DBGFrame::name()
{
    return m_name;
}

int DBGFrame::datasize()
{
    return m_datasize;
}

char* DBGFrame::toArray()
{
  clearRawData();
  m_raw = new char[DBGFrame::SIZE];

  m_raw = DBGNetHelper::toNetwork(m_name     , m_raw, 0);
  m_raw = DBGNetHelper::toNetwork(m_datasize , m_raw, 4);

  return m_raw;
}

