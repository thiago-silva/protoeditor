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

#ifndef DEBUGGERDBG_H
#define DEBUGGERDBG_H

#include "abstractdebugger.h"
#include <qvaluelist.h>

class DebuggerManager;
class DBGSettings;
class DBGNet;
class DebuggerStack;
class PHPVariable;
class DBGProfileDialog;

class DebuggerDBG : public AbstractDebugger
{
Q_OBJECT
public:
  DebuggerDBG(DebuggerManager*);
  virtual ~DebuggerDBG();

  /* Interface of AbstractDebugger */

  virtual QString name()   const;
  //virtual int     id() const;

  virtual bool isRunning() const;

  virtual void init();
  virtual void run(const QString&);
  virtual void continueExecution();
  virtual void stop();
  virtual void stepInto();
  virtual void stepOver();
  virtual void stepOut();

  virtual void addBreakpoints(const QValueList<DebuggerBreakpoint*>&);
  virtual void addBreakpoint(DebuggerBreakpoint*);
  virtual void changeBreakpoint(DebuggerBreakpoint*);
  virtual void removeBreakpoint(DebuggerBreakpoint*);
  virtual void modifyVariable(Variable* v, DebuggerExecutionPoint* execPoint);

  virtual void changeCurrentExecutionPoint(DebuggerExecutionPoint*);
  virtual void addWatch(const QString& expression);
  virtual void removeWatch(const QString& expression);

  virtual void profile(const QString&);

  /* Internal use (provided for DBGNet use) */
  void updateStack(DebuggerStack*);
  void updateVar(const QString& result, const QString& str, bool isGlobal);
  void updateWatch(const QString& result, const QString& str);
  void updateBreakpoint(int id, const QString& filePath, int line, int state, int hitcount, int skiphits,
                               const QString& condition);
  
  void addProfileData(int modid, const QString& filePath, int ctxid, const QString ctxname,
                      int line, long hitcount, double min, double max, double sum);
  
  void debugError(const QString& msg);
  void debugLog(int type, const QString& msg, int line, const QString& filePath, int extInfo);
  void checkDBGVersion(int, int, const QString&);

public slots:
  void slotSettingsChanged();

  void slotInternalError(const QString&);
  void slotDBGStarted();
  void slotDBGClosed(); //end of debug

private slots:
  void slotStepDone();
  void slotBreakpoint();
private:
  DBGProfileDialog* profileDialog();
    
  bool startJIT();
  void stopJIT();
  void requestWatches(int scopeid);
  int getDBGOptions();
  void processStepData();
//   void requestProfileData();

  QString                 m_name;
  bool                    m_isJITActive;
  bool                    m_isRunning;
//   bool                    m_isProfilingEnabled;
  
  DBGSettings            *m_dbgSettings;

  DBGNet                 *m_net;
  DBGProfileDialog       *m_profileDialog;
  int                     m_currentExecutionPointID;
  int                     m_globalExecutionPointID;
  //DebuggerExecutionPoint *m_currentExecutionPoint; //aways the top of the stack
  //DebuggerExecutionPoint *m_globalExecutionPoint;  //aways the bottom of the stack
//   QString                 m_output;
  QValueList<QString>     m_wathcesList;
  bool                    m_firstStep;
};


#endif
