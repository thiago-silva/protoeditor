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


#ifndef DBGREQUESTPACK_H
#define DBGREQUESTPACK_H

class QSocket;
class DBGFrame;
class DBGRequestTag;

#include <qvaluelist.h> //use QMap??
#include <qpair.h>

class DBGHeader;
class QSocket;

class DBGRequestPack{
public:
  DBGRequestPack(int cmd);
  //DBGRequestPack();
  ~DBGRequestPack();

  void addInfo(DBGFrame* frame, DBGRequestTag* dbgTag);
  void send(QSocket* socket);

  DBGHeader* header();

private:
  int calcBodySize();
  //typedef std::multimap<DBGFrame*, DBGRequestTag*> map_t;
  typedef QPair<DBGFrame*, DBGRequestTag*> FrameReqPair_t;
  typedef QValueList<FrameReqPair_t> List_t;
  List_t m_list;
  DBGHeader* m_header;
};

#endif
