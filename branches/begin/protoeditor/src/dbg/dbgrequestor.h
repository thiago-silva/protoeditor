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


#ifndef DBGREQUESTOR_H
#define DBGREQUESTOR_H

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>

class DebuggerBreakpoint;
class DBGRequestPack;
class QSocket;
class QHttp;

class DBGRequestor : public QObject {
Q_OBJECT
public:
  DBGRequestor();
   ~DBGRequestor();

  void requestContinue();
  void requestStop();
  void requestStepInto();
  void requestStepOver();
  void requestStepOut();
  void requestWatch(const QString& expression, int scope_id);
  void requestVariables(int scope_id);
  void requestSrcTree();
  void makeHttpRequest(const QString& host, const QString& path, int listenPort, int sessionId);
  void requestBreakpoint(int modno, DebuggerBreakpoint*);
  void requestBreakpointList(int bpno);
  void requestBreakpointRemoval(DebuggerBreakpoint* bp);
  void requestOptions(int op);

  void setSocket(QSocket* socket);

private slots:
  void slotHttpDone(bool error);
  void slotClosed();
  //void slotBytesWritten(int bytes);

signals:
  void requestorError(const QString& error);

private:
  void flushData();
  void clearPack(DBGRequestPack*);
  void deletePacks();
  //making a list of DBGRequestPack because we can't delete the pack
  //right after sending it.
  //DBGRequestPack* m_requestPack;

  QPtrList<DBGRequestPack> m_deleteList;

  QSocket      *m_socket;
  QHttp        *m_http;
  bool          m_terminating;
};

#endif
