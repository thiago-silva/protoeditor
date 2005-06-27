/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
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

#ifndef XDNET_H
#define XDNET_H

#include <qobject.h>
#include <qstring.h>

class DebuggerXD;
class SiteSettings;
class DebuggerBreakpoint;
class Connection;
class Browser;

class QSocket;
class QDomElement;
class QSocket;

class XDNet : public QObject
{
  Q_OBJECT
public:
  static const int GLOBAL_SCOPE;
  static const int LOCAL_SCOPE;

  XDNet(DebuggerXD* debugger, QObject *parent = 0, const char *name = 0);
  ~XDNet();

  //DebuggerXD communicates trough the following:
  bool startListener(int port);
  void stopListener();

  void startDebugging(const QString& filePath, SiteSettings* site);
//   void startDebugging(const QString& filePath, SiteSettings* site);
//   void startProfiling(const QString& filePath, SiteSettings* site);

  void requestContinue();
  void requestStop();
  void requestStepInto();
  void requestStepOver();
  void requestStepOut();

  void requestWatch(const QString& expression, int ctx_id = 0);
  void requestVariables(int scope, int id);

  void requestBreakpoint(DebuggerBreakpoint* bp);
  void requestBreakpointUpdate(DebuggerBreakpoint* bp);
  void requestBreakpointRemoval(int bpid);

signals:
  void sigError(const QString&);
  void sigXDStarted();
  void sigXDClosed();
  void sigStepDone();
//   void sigBreakpoint();

  /*
  private slots:
  void slotIncomingConnection(QSocket*);
  void slotDBGClosed();
  void slotError(const QString&);
  */

private slots:
  void slotIncomingConnection(QSocket*);
  void slotXDClosed();
  void slotError(const QString&);
  
//   void slotHttpDone(bool error);
  void slotReadBuffer();
private:
  void makeHttpRequest(const QString& _url, const QString& path);
//   void requestPage(const QString& filePath, SiteSettings* site);
  void requestStack();
  void requestBreakpointList();

  void processOutput(QDomElement&);
  void processError(const QDomElement&);
  
  void processXML(const QString& xml);
  
  void processInit(QDomElement& root);
  void processResponse(QDomElement& root);

  void processPendingData();
  
  void error(const QString&);

  DebuggerXD       *m_debugger;
  Connection       *m_con;

  Browser* m_browser;
//   QHttp *m_http;
  QSocket* m_socket;

  
  class Error {
    public:
      bool exists;
      QString code;
      QString data;
      QString exception;
  };

  Error            m_error; //php error
};

#endif
