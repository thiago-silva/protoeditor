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


#include "dbgresponsepack.h"
#include "dbgtags.h"
#include "dbgheader.h"
#include "dbg_defs.h"
#include <qsocket.h>

DBGResponsePack::DBGResponsePack()
{
}

DBGResponsePack::~DBGResponsePack()
{
  for(DBGBaseTag *baseTag = m_baseTags.first(); baseTag ; baseTag = m_baseTags.next() ) {
    delete baseTag;
  }

  for(DBGBaseTag *rawTag = m_rawTags.first(); rawTag ; rawTag = m_rawTags.next() ) {
    delete rawTag;
  }
}

void DBGResponsePack::addTag(DBGBaseTag* tag)
{
  if(tag->name() == FRAME_RAWDATA)
  {
    m_rawTags.append(tag);
  }
  else
  {
    m_baseTags.append(tag);
  }
}

bool DBGResponsePack::has(int framename)
{
  for(DBGBaseTag *tag = m_baseTags.first(); tag ; tag = m_baseTags.next() ) {
    if(tag->name() == framename) {
      return true;
    }
  }
  return false;
}

DBGBaseTag* DBGResponsePack::retrieve(int framename)
{
  for(DBGBaseTag *tag = m_baseTags.first(); tag ; tag = m_baseTags.next() ) {
    if(tag->name() == framename) {
      return tag;
    }
  }
  return NULL;
}

DBGResponsePack::StackTagList_t DBGResponsePack::retrieveStackTagList()
{
  StackTagList_t stackList;
  DBGResponseTagStack* stackTag;

  for(DBGBaseTag *tag = m_baseTags.first(); tag ; tag = m_baseTags.next() )
  {
    if(tag->name() == FRAME_STACK)
    {
      stackTag = dynamic_cast<DBGResponseTagStack*>(tag);
      StackTagPair_t p(stackTag, retrieveRawdata(stackTag->idescr()));
      stackList.append(p);
    }
  }

  return stackList;
}

DBGResponsePack::BpTagMap_t DBGResponsePack::retrieveBpTagMap() {
  BpTagMap_t map;

  DBGTagBreakpoint* bpTag;

  for(DBGBaseTag *tag = m_baseTags.first(); tag ; tag = m_baseTags.next() )
  {
    if(tag->name() == FRAME_BPS)
    {
      bpTag = dynamic_cast<DBGTagBreakpoint*>(tag);
      RawPair_t  p(retrieveRawdata(bpTag->imodname()),
        retrieveRawdata(bpTag->icondition()));
      map[bpTag] = p;
    }
  }

  return map;
}

DBGResponsePack::TreeStackList_t DBGResponsePack::retrieveTreeTagList()
{
  TreeStackList_t treeList;
  DBGResponseTagSrcLinesInfo* srcTag;

  for(DBGBaseTag *tag = m_baseTags.first(); tag ; tag = m_baseTags.next() )
  {
    srcTag = dynamic_cast<DBGResponseTagSrcLinesInfo*>(tag);
    TreeStackPair_t p(srcTag, retrieveRawdata(srcTag->imodName()));
    treeList.append(p);
  }

  return treeList;
}

DBGTagRawdata* DBGResponsePack::retrieveRawdata(int id)
{
  for(DBGBaseTag *tag = m_rawTags.first(); tag ; tag = m_rawTags.next() )
  {
    DBGTagRawdata* raw = dynamic_cast<DBGTagRawdata*>(tag);
    if(raw->id() == id)
    {
      return raw;
    }
  }
  return NULL;
}
