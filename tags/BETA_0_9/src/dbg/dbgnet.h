/***************************************************************************
 *   Copyright (C) 2004-2006 by Thiago Silva                               *
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
class Connection;
class DBGReceiver;
class DBGRequestor;
class QSocket;
class DBGResponsePack;
class DBGHeader;
class DBGStack;
class DBGFileInfo;
class DebuggerBreakpoint;

class DBGResponseTagSid;
class DBGResponseTagStack;
class DBGResponseTagVersion;
class DBGResponseTagSrcTree;
class DBGResponseTagEval;
class DBGResponseTagLog;
class DBGResponseTagError;
class DBGTagBreakpoint;
class DBGResponseTagProf;
class DBGResponseTagProfC;
class DBGResponseTagSrcLinesInfo;
class DBGResponseTagSrcCtxInfo;

class SiteSettings;

class DBGNet : public QObject
{
  Q_OBJECT
public:
  DBGNet(DebuggerDBG* debugger, QObject *parent = 0, const char *name = 0);
  ~DBGNet();

  //DebuggerDBG communicates trough the following:
  bool startListener(int port);
  void stopListener();

  void setSite(SiteSettings* site);
  void startDebugging(const QString&, const QString& uiargs,
      SiteSettings* site, bool local, int listenPort, dbgint sessid);

  void startProfiling(const QString& filePath, const QString& uiargs,
      SiteSettings* site, bool local, int listenPort, dbgint sessid);
  
  void requestOptions(dbgint options);

  void requestContinue();
  void requestStop();
  void requestStepInto();
  void requestStepOver();
  void requestStepOut();

  void requestVariables(dbgint scopeid, bool isGglobal);
  void requestWatch(const QString& expression, dbgint scopeid);
  void requestBreakpoint(DebuggerBreakpoint* bp);
  void requestBreakpointRemoval(int bpid);
  
  //DBGReceiver communicatDBGNetes through those:
  void receivePack(DBGResponsePack* pack);
  void receiveHeader(DBGHeader* header);

  //tags communicate trought those:

  void processSessionId(const DBGResponseTagSid* sid, DBGResponsePack* pack);
  void processStack(const DBGResponseTagStack* stack, DBGResponsePack* pack);
  void processDBGVersion(const DBGResponseTagVersion* version, DBGResponsePack* pack);
  void processSrcTree(const DBGResponseTagSrcTree* src, DBGResponsePack* pack);
  void processSrcLinesInfo(const DBGResponseTagSrcLinesInfo* src, DBGResponsePack* pack);
  void processSrcCtxInfo(const DBGResponseTagSrcCtxInfo*, DBGResponsePack* pack);
  void processEval(const DBGResponseTagEval* eval, DBGResponsePack* pack);

  void processLog(const DBGResponseTagLog* log, DBGResponsePack* pack);
  void processError(const DBGResponseTagError*, DBGResponsePack* pack);

  void processBreakpoint(const DBGTagBreakpoint*, DBGResponsePack* pack);

  void processProf(const DBGResponseTagProf*, DBGResponsePack* pack);
  void processProfC(const DBGResponseTagProfC*, DBGResponsePack* pack);
  
  void setOptions(int);
signals:
  void sigError(const QString&);
  void sigDBGStarted();
  void sigDBGClosed();
  void sigStepDone();
  void sigBreakpoint();
  void sigNewConnection();
private slots:
  void slotIncomingConnection(QSocket*);
  void slotDBGClosed();
  void slotError(const QString&);

private:
  void requestScript(const QString& filePath, const QString& uiargs, 
    SiteSettings* site, bool local, int listenPort, dbgint sessid);

  void processStepData();
  void processProfileData();
  bool processHeader(DBGHeader* header);
  void shipStack();
  void setupProfile();

  void error(const QString&);

  dbgint             m_opts;
  dbgint             m_sessionId;
  dbgint             m_headerFlags;
  dbgint             m_profFreq;
  DebuggerDBG       *m_debugger;
  Connection        *m_con;
  DBGReceiver       *m_receiver;
  DBGRequestor      *m_requestor;

  DBGStack          *m_dbgStack;
  DBGFileInfo       *m_dbgFileInfo;
  bool              m_isProfiling;
  bool              m_setupProfile;
  QValueList<dbgint> m_varScopeRequestList; //so we know wich context the vars belong
};

#endif
