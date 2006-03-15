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

#ifndef DEBUGGERGB_H
#define DEBUGGERGB_H

#include <abstractdebugger.h>
#include <qmap.h>

class DebuggerManager;
class GBSettings;
class GBNet;

class DebuggerGB : public AbstractDebugger
{
  Q_OBJECT
public:
  DebuggerGB(DebuggerManager*);
  virtual ~DebuggerGB();

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

  virtual void addWatches(const QStringList&);
  virtual void addWatch(const QString& expression);
  virtual void removeWatch(const QString& expression);

  virtual void profile(const QString&, bool local);


  GBSettings* settings();

  void updateStack(DebuggerStack* stack);
  void updateVars(const QString& scope, const QString& vars);
  void updateWatch(const QString&, const QString&);

public slots:
  void slotSettingsChanged();
private slots:
  void slotGBStarted();
  void slotGBClosed();
  void slotStepDone();

private:
  bool startJIT();
  void stopJIT();

  void requestWatches();
  
  QString m_name;
  bool m_isRunning;
  bool m_isJITActive;
  int m_listenPort;

  DebuggerExecutionPoint* m_currentExecutionPoint;
  DebuggerExecutionPoint* m_globalExecutionPoint;

  GBSettings* m_gbSettings;
  GBNet* m_net;
  QValueList<QString>  m_wathcesList;
};

#endif
