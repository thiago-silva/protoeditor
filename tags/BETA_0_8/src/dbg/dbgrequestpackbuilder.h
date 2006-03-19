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

#ifndef DBGREQUESTPACKBUILDER_H
#define DBGREQUESTPACKBUILDER_H
#include <qstring.h>
#include "dbgrequestpack.h"

class DBGPack;
class DebuggerBreakpoint;

class DBGRequestPackBuilder
{
public:
  static DBGRequestPack* buildCommand(int cmd);
  static DBGRequestPack* buildWatch(const QString expression, int scope_id);
  //static DBGRequestPack* buildBreakpoint(int modno, DebuggerBreakpoint* breakpoint);
  static DBGRequestPack* buildBreakpoint(int bpno, int modno, const QString& remoteFilePath,
                                         int line, const QString& condition, int status, int skiphits, bool istemp);
  //static DBGRequestPack* buildBreakpointList(int bpno);
  static DBGRequestPack* buildDeletedBreakpoint(int bpid);
  static DBGRequestPack* buildVars(int mod_no);
  static DBGRequestPack* buildSrcTree();
  static DBGRequestPack* buildSrcLinesInfo(int modno);
  static DBGRequestPack* buildSrcCtxInfo(int modno);
  static DBGRequestPack* buildOptions(int op);
  static DBGRequestPack* buildProfile(int modno);
  static DBGRequestPack* buildProfileC(int testLoops);
};


#endif
