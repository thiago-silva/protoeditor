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

#ifndef DBGNET_H
#define DBGNET_H

#include <qobject.h>
#include <qvaluelist.h>
#include "dbg_defs.h"

class DebuggerDBG;
class DBGConnection;
class DBGReceiver;
class DBGRequestor;
class QSocket;
class DBGResponsePack;
class DBGHeader;
class DBGStack;

class DBGNet : public QObject
{
  Q_OBJECT
public:
  DBGNet(DebuggerDBG* debugger, QObject *parent = 0, const char *name = 0);
  ~DBGNet();

  //DebuggerDBG communicates trough the following:
  bool startListener(int port);
  void stopListener();

  void requestPage(const QString& host, const QString& filePath, int port, dbgint sessid);
  void requestOptions(dbgint options);

  void requestContinue();
  void requestStop();
  void requestStepInto();
  void requestStepOver();
  void requestStepOut();

  void requestLocalVariables(dbgint scopeid);
  void requestGlobalVariables();
  void requestWatch(const QString& expression, dbgint scopeid);

  //DBGReceiver communicates through those:
  void receivePack(DBGResponsePack* pack);
  void receiveHeader(DBGHeader* header);

  //tags communicate trought those:

  void processSessionId(dbgint sessiontype, const QString& sessionId);
  void processStack(dbgint modno,const QString& descr,dbgint lineno, dbgint scopeid);
  void processDBGVersion(dbgint majorv, dbgint minorv, const QString& descr);
  void processSrcTree(dbgint modno, dbgint parentlineno, dbgint parentmodno, const QString& modname);
  void processEval(const QString& result, const QString& str, const QString& error);


  void processLog(dbgint type, const QString& log, dbgint modno, dbgint line, const QString& modname, dbgint extInfo);
  void processError(dbgint type, const QString& msg);
  void processBreakpoint(dbgint modno, dbgint lineno, const QString& modname,
                         dbgint state, dbgint istemp, dbgint hitcount, dbgint skiphits,
                         const QString& condition, dbgint bpno, dbgint isunderhit);


signals:
  void sigError(const QString&);
  void sigDBGStarted();
  void sigDBGClosed();

private slots:
  void slotIncomingConnection(QSocket*);
  void slotDBGClosed();
  void slotError(const QString&);

private:
  bool processHeader(DBGHeader* header);
  void RequestCommonData();

  void updatePendingData();

  void error(const QString&);


  dbgint             m_sessionId;
  dbgint             m_headerFlags;
  DebuggerDBG       *m_debugger;
  DBGConnection     *m_con;
  DBGReceiver       *m_receiver;
  DBGRequestor      *m_requestor;

  DBGStack          *m_dbgStack;
  QValueList<dbgint> m_varScopeRequestList; //so we know wich context the vars belong

};

#endif
