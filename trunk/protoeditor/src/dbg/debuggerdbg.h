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
#include <qvaluelist.h>

class DebuggerManager;
class DBGConfiguration;
class DBGNet;
class DebuggerStack;
class PHPVariable;

class DebuggerDBG : public AbstractDebugger
{
Q_OBJECT
public:
  DebuggerDBG(DebuggerManager*);
  virtual ~DebuggerDBG();

  /* Interface of AbstractDebugger */

  virtual QString name()   const;
  virtual int     id() const;

  virtual bool isSessionActive() const;
  virtual bool isRunning() const;

  virtual void reloadConfiguration();

  virtual void startSession();
  virtual void endSession();
  virtual void run(const QString&);
  virtual void stop();
  virtual void stepInto();
  virtual void stepOver();
  virtual void stepOut();

  virtual void addBreakpoints(const QValueList<DebuggerBreakpoint*>&);
  virtual void addBreakpoint(DebuggerBreakpoint*);
  virtual void changeBreakpoint(DebuggerBreakpoint*);
  virtual void removeBreakpoint(DebuggerBreakpoint*);
  virtual void modifyVariable(Variable* v, DebuggerExecutionPoint* execPoint);

  virtual void requestLocalVariables(DebuggerExecutionPoint*);
  virtual void addWatch(const QString& expression);
  virtual void removeWatch(const QString& expression);

  /* Internal use (provided for DBGNet use) */
  void updateStack(DebuggerStack*);
  void updateVar(const QString& result, const QString& str, long scope);
  void updateWatch(const QString& result, const QString& str);
  void updateBreakpoint(int id, const QString& filePath, int line, int state, int hitcount, int skiphits,
                               const QString& condition);
  void debugError(const QString& msg);
  void debugLog(int type, const QString& msg, int line, const QString& filePath, int extInfo);
  void checkDBGVersion(int, int, const QString&);

  DBGConfiguration* configuration();

public slots:
  void slotInternalError(const QString&);
  void slotDBGStarted();
  void slotDBGClosed(); //end of debug

private slots:
  void slotStepDone();
private:
  void requestWatches(DebuggerExecutionPoint*);

  bool                    m_isSessionActive;
  bool                    m_isRunning;

  DBGConfiguration       *m_configuration;

  DBGNet                 *m_net;
  DebuggerExecutionPoint *m_currentExecutionPoint;
  QString                 m_output;
  QValueList<QString>     m_wathcesList;
};


#endif
