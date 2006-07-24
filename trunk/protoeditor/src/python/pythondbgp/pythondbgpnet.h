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

#ifndef PYTHONDBGPNET_H
#define PYTHONDBGPNET_H

#include "variable.h"

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h> 

class DebuggerPythonDBGP;
class SiteSettings;
class DebuggerBreakpoint;
class Connection;

class QSocket;
class QDomElement;
class QSocket;
class KURL;

class PythonDBGPNet : public QObject
{
  Q_OBJECT
public:
  //transaction id's
  enum {
    GeneralId = 1, //ID used for requesting common data
    TempBpId,  //ID used for requesting temp breakpoints
    RunToCursorId, //ID used to request "run to cursor"

    LocalScopeId, //ID for requesting local variables
    GlobalScopeId, //ID for requesting global variables
    SuperGlobalId, //ID for requesting super globals

    ChildId,       //ID for requesting variable children

    ErrorStackId //ID for requesting Stack to build error data, only
  };  

  PythonDBGPNet(DebuggerPythonDBGP* debugger, QObject *parent = 0, const char *name = 0);
  ~PythonDBGPNet();

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

  void requestRunToCursor(const QString&, int);
  void requestStepInto();
  void requestStepOver();
  void requestStepOut();

  void requestWatch(const QString& expression, int ctx_id = 0);
  void requestChildren(int, Variable*);

  void requestVariables(int scope, int id);
  void requestSuperGlobals(int scope);

  void requestModifyVar(Variable*, int);

  void requestBreakpoint(DebuggerBreakpoint* bp);
  void requestBreakpointUpdate(DebuggerBreakpoint* bp);
  void requestBreakpointRemoval(int bpid);

 
signals:
  void sigError(const QString&);
  void sigStarted();
  void sigClosed();
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
  void slotClosed();
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

  void sendCommand(const QString& cmd, int len);

  Variable           *m_updateVar;
  long                m_tempBreakpointId;  
  SiteSettings       *m_site;
  DebuggerPythonDBGP *m_debugger;
  Connection         *m_con;

  QSocket            *m_socket;

  VariableList_t* m_globalVars;
  int m_superglobalsCount;
  
  class Error {
    public:
      QString code;
      QString filePath;
      int line;
      QString data;
      QString exception;
  };

  Error                  m_error; //python error  
  QPtrList<Variable>     m_updateVars;
};

#endif
