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

#include "perldebugger.h"
#include "perlsettings.h"
#include "protoeditorsettings.h"
#include "debuggermanager.h"

PerlDebugger::PerlDebugger(DebuggerManager* manager)
  : AbstractDebugger(manager), m_name("Perl debugger"), m_isRunning(false), m_isJITActive(false)
{
//   m_currentExecutionPoint = new DebuggerExecutionPoint();
//   m_globalExecutionPoint = new DebuggerExecutionPoint();

//   m_perlSettings = new PerlSettings(m_name);

//   ProtoeditorSettings::self()->registerDebuggerSettings(m_perlSettings, m_name);

  connect(ProtoeditorSettings::self(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));
}

PerlDebugger::~PerlDebugger()
{
}

QString PerlDebugger::name()   const
{ 
  return m_name;
}

bool PerlDebugger::isRunning() const
{
  return m_isRunning;
}


void PerlDebugger::init()
{
}


void PerlDebugger::start(const QString&, const QString& args, bool local)
{
}

void PerlDebugger::continueExecution()
{
}

void PerlDebugger::stop()
{
}

void PerlDebugger::stepInto()
{
}

void PerlDebugger::stepOver()
{
}

void PerlDebugger::stepOut()
{
}

void PerlDebugger::runToCursor(const QString&, int)
{
}


void PerlDebugger::addBreakpoints(const QValueList<DebuggerBreakpoint*>&)
{
}

void PerlDebugger::addBreakpoint(DebuggerBreakpoint*)
{
}


void PerlDebugger::changeBreakpoint(DebuggerBreakpoint*)
{
}

void PerlDebugger::removeBreakpoint(DebuggerBreakpoint*)
{
}

void PerlDebugger::changeCurrentExecutionPoint(DebuggerExecutionPoint*)
{
}

void PerlDebugger::modifyVariable(Variable* v, DebuggerExecutionPoint*)
{
}

void PerlDebugger::addWatches(const QStringList&)
{
}

void PerlDebugger::addWatch(const QString& expression)
{
}

void PerlDebugger::removeWatch(const QString& expression)
{
}

void PerlDebugger::profile(const QString&, const QString& args, bool local)
{
}

//---------------


void PerlDebugger::slotSettingsChanged()
{
}
#include "perldebugger.moc"
