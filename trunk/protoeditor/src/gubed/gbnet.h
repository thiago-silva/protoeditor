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
class QSocket;

typedef QMap<QString,QString> StringMap;

class GBNet : public QObject
{
  Q_OBJECT
public:
  GBNet(DebuggerGB* debugger, QObject *parent = 0, const char *name = 0);
  ~GBNet();

  //DebuggerGB communicates trough the following:
  bool startListener(int port);
  void stopListener();

  void startDebugging(const QString& filePath, SiteSettings* site, bool local);

  void requestContinue();
  void requestStop();
  void requestStepInto();
  void requestStepOver();
  void requestStepOut();

  void requestWatches(const QStringList&);
  void requestWatch(const QString& expression);
  //   void requestVariables(int scope, int id);
  //
  //   void requestBreakpoint(DebuggerBreakpoint* bp);
  //   void requestBreakpointUpdate(DebuggerBreakpoint* bp);
  //   void requestBreakpointRemoval(int bpid);

signals:
  void sigError(const QString&);
  void sigGBStarted();
  void sigGBClosed();
  void sigStepDone();

private slots:
  void slotIncomingConnection(QSocket*);
  void slotGBClosed();
  void slotError(const QString&);

  void slotReadBuffer();
private:
//   QString getField(int* idx, QString text);
            
  void error(const QString&);

  void processCommand(const QString& datas);
  StringMap parseArgs(const QString &args);
  bool sendCommand(const QString& command, QMap<QString, QString> args);
  bool sendCommand(const QString& command, char * firstarg, ...);
  QString phpSerialize(QMap<QString, QString> args);


  void processBacktrace(const QString& bt);
  void processVariable(const QString& var);
  void processVariables(const QString& vars);  

  DebuggerGB       *m_debugger;
  Connection       *m_con;  
  QSocket* m_socket;
  long             m_datalen;

  QString m_command;
};

#endif
