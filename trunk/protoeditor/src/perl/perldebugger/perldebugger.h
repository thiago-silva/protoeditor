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
#ifndef PERLDEBUGGER_H
#define PERLDEBUGGER_H

#include "abstractdebugger.h"

#include <qstring.h>

class PerlDebugger : public AbstractDebugger
{
  Q_OBJECT
public:
  PerlDebugger();
  virtual ~PerlDebugger();

  virtual QString name()   const;
  virtual bool isRunning() const;

  virtual void init();

  virtual void start(const QString&, const QString& args, bool local);
  virtual void continueExecution();
  virtual void stop();
  virtual void stepInto();
  virtual void stepOver();
  virtual void stepOut();

  virtual void runToCursor(const QString&, int);

  virtual void addBreakpoints(const QValueList<DebuggerBreakpoint*>&);

  virtual void addBreakpoint(DebuggerBreakpoint*);

  virtual void changeBreakpoint(DebuggerBreakpoint*);
  virtual void removeBreakpoint(DebuggerBreakpoint*);

  virtual void changeCurrentExecutionPoint(DebuggerExecutionPoint*);

  virtual void modifyVariable(Variable* v, DebuggerExecutionPoint*);

  virtual void addWatches(const QStringList&);
  virtual void addWatch(const QString& expression);
  virtual void removeWatch(const QString& expression);

  virtual void profile(const QString&, const QString& args, bool local);

private slots:
  void slotSettingsChanged();

private:

  QString m_name;

  bool m_isRunning;
  bool m_isJITActive;
  
//   PerlSettings *m_perlSettings;
};

#endif
