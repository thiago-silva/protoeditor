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


#ifndef DBGFRAME_H
#define DBGFRAME_H
//#include "dbgrequestdatainterface.h"
#include "dbgabstractdatarequest.h"

class BaseTag;

class DBGFrame : public DBGAbstractDataRequest {
public:
  DBGFrame();
  DBGFrame(int name, int datasize);
  DBGFrame(char* responseBuffer);
  virtual ~DBGFrame();

  void setName(int name);
  void setDatasize(int datasize);

  int name();
  int datasize();

  virtual char* toArray();

  static const int SIZE;
private:
  int      m_name;
  int      m_datasize;
};

#endif
