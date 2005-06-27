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

#ifndef GBNET_H
#define GBNET_H

#include <qobject.h>
#include <qmap.h>

class DebuggerGB;
class SiteSettings;
class Connection;
class Browser;
class QSocket;

class GBNet : public QObject
{
  Q_OBJECT
public:
  GBNet(DebuggerGB* debugger, QObject *parent = 0, const char *name = 0);
  ~GBNet();

  //DebuggerGB communicates trough the following:
  bool startListener(int port);
  void stopListener();

  void startDebugging(const QString& filePath, SiteSettings* site);

  void requestContinue();
  void requestStop();
  void requestStepInto();
  void requestStepOver();
  void requestStepOut();

  //   void requestWatch(const QString& expression, int ctx_id = 0);
  //   void requestVariables(int scope, int id);
  //
  //   void requestBreakpoint(DebuggerBreakpoint* bp);
  //   void requestBreakpointUpdate(DebuggerBreakpoint* bp);
  //   void requestBreakpointRemoval(int bpid);

  void sendRunMode(int mode);
  void sendErrorSettings(int errorno);
  void sendHaveSource(bool have, const QString& file);
  void requestBacktrace();
  void sendWait();
  void sendNext();
signals:
  void sigError(const QString&);
  void sigGBStarted();
  void sigGBClosed();
  //     void sigStepDone();

private slots:
  void slotIncomingConnection(QSocket*);
  void slotGBClosed();
  void slotError(const QString&);

  void slotReadBuffer();
private:
  QString readSocket();
      
  typedef QMap<QString,QString> StringMap_t;
  
  void sendCommand(const QString&, const StringMap_t& = StringMap_t());
  QString serialize(const StringMap_t&);
  StringMap_t unserialize(const QString& text);
  QString getField(int* idx, QString text);
            
  void makeHttpRequest(const QString& _url, const QString& path);

  /*    void requestStack();
      void requestBreakpointList();
   
      void processOutput(QDomElement&);
      void processError(const QDomElement&);
    
      void processXML(const QString& xml);
    
      void processInit(QDomElement& root);
      void processResponse(QDomElement& root);
   
      void processPendingData();*/

  void error(const QString&);

  DebuggerGB       *m_debugger;
  Connection       *m_con;

  Browser* m_browser;
  QSocket* m_socket;
};

#endif
