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

#ifndef DEBUGGERSTACK_H
#define DEBUGGERSTACK_H

#include <qptrlist.h>
#include <qstring.h>

class DebuggerExecutionPoint {
public:
  DebuggerExecutionPoint();
  DebuggerExecutionPoint(int id, QString filePath, int line, QString function);
  ~DebuggerExecutionPoint();

  void setId(int);
  int  id();

  void setFilePath(QString);
  QString filePath();

  void setLine(int);
  int line();

  void setFunction(QString);
  QString function();
private:
  int m_id;
  QString m_filePath;
  int m_line;
  QString m_function;
};

//---------------------------------------------------------------------

class DebuggerStack{
public:
  typedef QPtrList<DebuggerExecutionPoint> DebuggerExecutionPointList_t;
  DebuggerStack();
  ~DebuggerStack();

  void push(int id, QString filePath, int line, QString function);
  void push(DebuggerExecutionPoint*);

  DebuggerExecutionPoint* topExecutionPoint();
  DebuggerExecutionPointList_t DebuggerExecutionPointList();
private:
  DebuggerExecutionPointList_t m_execPointList;
};

#endif
