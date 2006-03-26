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

#include "variable.h"

#include <qobject.h>
#include <qstring.h>

class DebuggerXD;
class SiteSettings;
class DebuggerBreakpoint;
class Connection;

class QSocket;
class QDomElement;
class QSocket;
class KURL;

class XDNet : public QObject
{
  Q_OBJECT
public:
  //transaction id's
  enum {
    GeneralId, //ID used for requesting common data

    LocalScopeId, //ID for requesting local variables
    GlobalScopeId, //ID for requesting global variables
    SuperGlobalId, //ID for requesting super globals

    ErrorStackId //ID for requesting Stack to build error data, only
  };

  XDNet(DebuggerXD* debugger, QObject *parent = 0, const char *name = 0);
  ~XDNet();

  void setSite(SiteSettings* site);

  //DebuggerXD communicates trough the following:
  bool startListener(int port);
  void stopListener();

  void startDebugging(const QString& filePath, const QString& uiargs,
    SiteSettings* site, bool local);
//   void startDebugging(const QString& filePath, SiteSettings* site);
//   void startProfiling(const QString& filePath, SiteSettings* site);

  void requestContinue();
  void requestStop();
  void requestStepInto();
  void requestStepOver();
  void requestStepOut();

  void requestWatch(const QString& expression, int ctx_id = 0);
  void requestVariables(int scope, int id);
  void requestSuperGlobals(int scope);

  void requestBreakpoint(DebuggerBreakpoint* bp);
  void requestBreakpointUpdate(DebuggerBreakpoint* bp);
  void requestBreakpointRemoval(int bpid);

signals:
  void sigError(const QString&);
  void sigXDStarted();
  void sigXDClosed();
  void sigStepDone();
  void sigNewConnection();
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
  void requestProperty(const QString& expression, int ctx_id, int id);

  void requestStack(int id);
  void requestBreakpointList();

  void processOutput(QDomElement&);
  void processError(const QDomElement&);
  
  void processXML(const QString& xml);
  
  void processInit(QDomElement& root);
  void processResponse(QDomElement& root);

  void dispatchErrorData();
  
  void error(const QString&);

  SiteSettings     *m_site;
  DebuggerXD       *m_debugger;
  Connection       *m_con;

  QSocket          *m_socket;

  VariablesList_t* m_globalVars;
  int m_superglobalsCount;
  
  class Error {
    public:
      QString code;
      QString filePath;
      int line;
      QString data;
      QString exception;
  };

  Error            m_error; //php error  
};

#endif
