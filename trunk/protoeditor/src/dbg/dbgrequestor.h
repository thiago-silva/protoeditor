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


#ifndef DBGREQUESTOR_H
#define DBGREQUESTOR_H

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>
#include "dbg_defs.h"

class DBGRequestPack;
class QSocket;
class Browser;

class DBGRequestor : public QObject
{
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
  void requestSrcLinesInfo(int modno);
  void requestSrcCtxInfo(int modno);
  //void requestBreakpoint(int modno, DebuggerBreakpoint*);
  void requestBreakpoint(int bpno, int modno, const QString& remoteFilePath, int line, const QString& condition, int status, int skiphits, bool istemp = false);

  //void requestBreakpointList(int bpno);
  void requestBreakpointRemoval(int bpid);
  void requestOptions(int op);

  void requestProfileData(int modno);
  void requestProfileFreqData(int testLoops);
  
  void makeHttpRequest(const QString& _url, /*int port, */const QString& path, int listenPort, int sessionId);

  void addHeaderFlags(dbgint);
  void setSocket(QSocket* socket);
  void clear();
private slots:
  //void slotHttpDone(bool error);

  //void readyRead(const QHttpResponseHeader&);

signals:
  void sigError(const QString& error);

private:

  QSocket      *m_socket;
  Browser      *m_browser;
  //QHttp        *m_http;
  dbgint        m_headerFlags;

};

#endif
