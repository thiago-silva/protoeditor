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

#ifndef DBGSTACK_H
#define DBGSTACK_H
#include <qvaluelist.h>
#include "dbgresponsepack.h"

class DebuggerStack;
class DBGFileInfo;

class DBGStack {
public:
  DBGStack();
  ~DBGStack();

  void setStackTagList(DBGResponsePack::StackTagList_t);
  void clear();

  DebuggerStack* debuggerStack(DBGFileInfo*);

private:
  typedef struct {
    QString filePath;
    QString function;
    int line;
    int count;
    int modNo;
    int scopeId;
  } StackInfo;

  DebuggerStack* m_stack;
  QValueList<StackInfo> m_stackInfoList; //temp used to get the filepaths
};

#endif
