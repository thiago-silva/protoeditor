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

#ifndef ABSTRACTDEBUGGER_H
#define ABSTRACTDEBUGGER_H

#include <qobject.h>
#include <qstring.h>
//#include "variable.h"

class DebuggerManager;
class DebuggerBreakpoint;
class DebuggerConfigurations;
class DebuggerExecutionPoint;
class DebuggerStack;
class Variable;

class SiteSettings;

class AbstractDebugger : public QObject {
Q_OBJECT
public:
  AbstractDebugger(QObject *parent, const char* name = 0);
  virtual ~AbstractDebugger();

  virtual QString name()   const = 0;
  //virtual int     id() const = 0;

  virtual bool isRunning()       const = 0;

  virtual void run(const QString&, SiteSettings*) = 0;
  virtual void stop()              = 0;
  virtual void stepInto()          = 0;
  virtual void stepOver()          = 0;
  virtual void stepOut()           = 0;

  virtual void addBreakpoints(const QValueList<DebuggerBreakpoint*>&) = 0;
  virtual void addBreakpoint(DebuggerBreakpoint*)               = 0;
  //virtual void addBreakpoint(const QString&, int)               = 0;
  virtual void changeBreakpoint(DebuggerBreakpoint*) = 0;
  virtual void removeBreakpoint(DebuggerBreakpoint*) = 0;

  //virtual void requestLocalVariables(DebuggerExecutionPoint*)       = 0;
  virtual void changeCurrentExecutionPoint(DebuggerExecutionPoint*) = 0;
  virtual void modifyVariable(Variable* v, DebuggerExecutionPoint*) = 0;

  virtual void addWatch(const QString& expression) = 0;
  virtual void removeWatch(const QString& expression) = 0;

protected:
  virtual DebuggerManager* manager();

signals:
  void sigDebugStarted();
  void sigDebugEnded();

  //Debugger client error (conection, listen port, etc)
  void sigInternalError(const QString&);
};

#endif
