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


#ifndef DBGHEADER_H
#define DBGHEADER_H
#include "dbgabstractdatarequest.h"

class DBGHeader : public DBGAbstractDataRequest {
public:
  DBGHeader();
  DBGHeader(char* responseBuffer);
  virtual ~DBGHeader();

  void setSync(int sync);
  void setCmd(int cmd);
  void setFlags(int flags);
  void setBodysize(int bodysize);

  int sync();
  int cmd();
  int flags();
  int bodysize();

  virtual char* toArray();

  static const int SIZE;
private:
  int m_sync;
  int m_cmd;
  int m_flags;
  int m_bodysize;
};

#endif
