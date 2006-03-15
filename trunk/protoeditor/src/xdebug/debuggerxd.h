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

#ifndef DEBUGGERXD_H
#define DEBUGGERXD_H

#include "abstractdebugger.h"
#include "variable.h"

#include <qvaluelist.h>
#include <qstring.h>

class XDSettings;
class XDNet;
class DebuggerExecutionPoint;

class DebuggerXD : public AbstractDebugger
{
  Q_OBJECT
public:
  DebuggerXD(DebuggerManager*);
  virtual ~DebuggerXD();

  virtual QString name() const;
  virtual bool isRunning() const;
  virtual void init();

  virtual void start(const QString&, bool local);
  virtual void continueExecution();
  virtual void stop();
  virtual void stepInto();
  virtual void stepOver();
  virtual void stepOut();

  virtual void addBreakpoints(const QValueList<DebuggerBreakpoint*>&);
  virtual void addBreakpoint(DebuggerBreakpoint*);
  virtual void changeBreakpoint(DebuggerBreakpoint*);
  virtual void removeBreakpoint(DebuggerBreakpoint*);

  virtual void changeCurrentExecutionPoint(DebuggerExecutionPoint*);
  virtual void modifyVariable(Variable* v, DebuggerExecutionPoint*);

  virtual void addWatch(const QString& expression);
  virtual void removeWatch(const QString& expression);

  virtual void profile(const QString&, bool local);

  //Communication with XDNet

  void updateStack(DebuggerStack* stack);
  void updateVariables(VariablesList_t*, bool);

  void updateWatch(Variable*);
  void updateBreakpoint(int id, const QString& filePath, int line, const QString& state, int hitcount, int skiphits,
                        const QString& condition);

  void addOutput(const QString&);
  void debugError(const QString& code, const QString& exception, const QString& data);
  
  XDSettings* settings();
public slots:
  void slotSettingsChanged();

private slots:
  void slotXDStarted();
  void slotXDStopped(); //end of debug
  void slotStepDone();
 
private:
  void requestVars();
  void requestWatches(int ctx_id);
  bool startJIT();
  void stopJIT();

  /*
  signals:
  void sigDebugStarted();
  void sigDebugEnded();
  void sigStepDone();
  void sigBreakpointReached();

  //Debugger client error (conection, listen port, etc)
  void sigInternalError(const QString&);
  */

  QString m_name;
  bool m_isRunning;
  bool m_isJITActive;
  int m_listenPort;

  DebuggerExecutionPoint* m_currentExecutionPoint;
  DebuggerExecutionPoint* m_globalExecutionPoint;
  XDSettings* m_xdSettings;
  XDNet* m_net;  
  QValueList<QString>  m_wathcesList;
};

#endif
