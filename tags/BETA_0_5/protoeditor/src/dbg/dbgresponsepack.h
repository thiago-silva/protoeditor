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


#ifndef DBGPACK_H
#define DBGPACK_H

#include <qvaluelist.h>
#include <qptrlist.h>
#include <qpair.h>
#include <qmap.h>

class DBGHeader;
class DBGBaseTag;
class DBGFrame;
class DBGTagRawdata;
class DBGResponseTagStack;
class DBGResponseTagSrcLinesInfo;
class DBGTagBreakpoint;

class DBGResponsePack {
public:

  //the order of the elements for the stack is important so, not using a map
  typedef QPair<DBGResponseTagStack*, DBGTagRawdata*> StackTagPair_t;
  typedef QValueList<StackTagPair_t> StackTagList_t;

  typedef QPair<DBGResponseTagSrcLinesInfo*, DBGTagRawdata*> TreeStackPair_t;
  typedef QValueList<TreeStackPair_t> TreeStackList_t;

  typedef QPair<DBGTagRawdata*, DBGTagRawdata*> RawPair_t;
  typedef QMap<DBGTagBreakpoint*, RawPair_t> BpTagMap_t;

  DBGResponsePack();
  ~DBGResponsePack();

  void addTag(DBGBaseTag* tag);
  bool has(int framename);

  DBGBaseTag*    retrieve(int framename);
  DBGTagRawdata* retrieveRawdata(int id);

  StackTagList_t   retrieveStackTagList();
  BpTagMap_t       retrieveBpTagMap();
  TreeStackList_t  retrieveTreeTagList();


private:
  typedef QPtrList<DBGBaseTag> TagList_t;

  TagList_t   m_baseTags;
  TagList_t   m_rawTags;
};


#endif
