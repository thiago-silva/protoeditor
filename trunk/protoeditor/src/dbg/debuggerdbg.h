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


#ifndef DEBUGGERDBG_H
#define DEBUGGERDBG_H

#include "abstractdebugger.h"
#include "dbgresponsepack.h"
#include <qmap.h>
#include <qvaluelist.h>
#include <qptrlist.h>

class DBGConfiguration;
class DebuggerManager;

class DBGRequestor;
class DBGReceiver;
class DBGConnection;
class DBGResponseTagStack;
class DBGResponseTagVersion;
class DBGResponseTagSid;
class DBGTagRawdata;
class DBGResponseTagEval;
class DBGResponsePack;
class DBGTagBreakpoint;
class DBGResponseTagLog;
class DBGResponseTagError;
class DebuggerStack;
class DBGStack;
class QSocket;
class DebuggerBreakpoint;
class DBGFileInfo;

class DebuggerDBG : public AbstractDebugger
{
Q_OBJECT
public:
  DebuggerDBG(DebuggerManager* manager);
  ~DebuggerDBG();

  virtual void startSession();
  virtual void endSession();
  virtual void run(QString);
  virtual void stop();
  virtual void stepInto();
  virtual void stepOver();
  virtual void stepOut();
  virtual QString name();
  virtual void loadConfiguration();

  virtual void addBreakpoints(QPtrList<DebuggerBreakpoint>);
  virtual void addBreakpoint(DebuggerBreakpoint*);
  //virtual void addBreakpoint(QString, int);
  virtual void changeBreakpoint(DebuggerBreakpoint*);
  virtual void removeBreakpoint(DebuggerBreakpoint*);


  virtual void requestLocalVariables(DebuggerExecutionLine* stackContext);
  virtual void modifyVariable(Variable*, DebuggerExecutionLine*);

  virtual void addWatch(QString , DebuggerExecutionLine*);
  virtual void removeWatch(QString expression);
  virtual void requestWatches(DebuggerExecutionLine* stackContext);

  void receivePack(DBGResponsePack* pack);

public slots:
  void slotError(const QString&);
  void slotIncomingConnection(QSocket* socket);
  void slotConnectionClosed();

private:
  //void initListener();

  void requestData();
  void requestGlobalVariables();
  //void requestBreakpoints();


  void updateSessionInfo(DBGResponseTagSid* sid, DBGTagRawdata* raw);
  void updateDebuggerVersion(DBGResponseTagVersion* version, DBGTagRawdata* raw);

  void updateBreakpoints(DBGResponsePack::BpTagMap_t bpMap);
  void sendBreakpoint(DBGTagBreakpoint* bpTag, DBGTagRawdata* fileraw, DBGTagRawdata* condraw);

  void updateVariables(DBGResponseTagEval* eval, DBGTagRawdata* result , DBGTagRawdata* istr, DBGTagRawdata* error);
  void updateStack();

  void showLog(DBGResponseTagLog* log, DBGTagRawdata* rawmod, DBGTagRawdata* rawlog);
  void showError(DBGResponseTagError* error, DBGTagRawdata* raw);

  void stopDebugger();

  void clearConnection();
  void internalError(QString error);

  //DebuggerBreakpoint* breakpoint(int id);

  DBGConnection     *m_con;
  DBGReceiver       *m_receiver;
  DBGRequestor      *m_requestor;

  DBGConfiguration  *m_configuration;
  DBGStack          *m_dbgStack;
  DBGFileInfo       *m_dbgFileInfo;

  int                m_currentSessionId;
  bool               m_initialized;
  QValueList<bool>   m_varScopeRequestList; //so we know wich context the vars belong

  QValueList<QString>          m_wathcesList;
  //QPtrList<VariableValue>      m_indexedVarList; //backup to use with watches

  //QPtrList<DebuggerBreakpoint> m_bpList;

};


#endif
