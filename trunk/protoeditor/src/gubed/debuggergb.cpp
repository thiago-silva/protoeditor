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

#include "debuggergb.h"
#include "debuggermanager.h"
#include "protoeditorsettings.h"

#include "gbsettings.h"
#include "gbnet.h"
#include "phpdefs.h"
#include "debuggerstack.h"

#include <kdebug.h>
#include <klocale.h>

#include "phpvariable.h"
#include "variableparser.h"


DebuggerGB::DebuggerGB(DebuggerManager* manager)
    : AbstractDebugger(manager), m_name("Gubed"), m_isRunning(false), m_isJITActive(false),
      m_listenPort(-1), m_currentExecutionPoint(0), m_globalExecutionPoint(0), 
      m_gbSettings(0), m_net(0)
{
  m_currentExecutionPoint = new DebuggerExecutionPoint();
  m_globalExecutionPoint = new DebuggerExecutionPoint();

  m_gbSettings = new GBSettings(m_name);

  ProtoeditorSettings::self()->registerDebuggerSettings(m_gbSettings, m_name);

  connect(ProtoeditorSettings::self(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  m_net = new GBNet(this);
  connect(m_net, SIGNAL(sigGBStarted()), this, SLOT(slotGBStarted()));
  connect(m_net, SIGNAL(sigGBClosed()), this, SLOT(slotGBClosed()));
  connect(m_net, SIGNAL(sigError(const QString&)), this, SIGNAL(sigInternalError(const QString&)));
  //   connect(m_net, SIGNAL(sigStepDone()), this, SLOT(slotStepDone()));
}


DebuggerGB::~DebuggerGB()
{
  delete m_gbSettings;
  delete m_net;
}

QString DebuggerGB::name() const
{
  return m_name;
}

bool DebuggerGB::isRunning() const
{
  return m_isRunning;
}

void DebuggerGB::init()
{
  slotSettingsChanged();
}


void DebuggerGB::start(const QString& filePath, bool local)
{
  SiteSettings* site  = ProtoeditorSettings::self()->currentSiteSettings();

  //we need JIT active (listening on port)
  if(!m_isJITActive && !startJIT())
  {
    return;
  }

  emit sigDebugStarting();

  m_net->startDebugging(filePath, site, local);
}

GBSettings* DebuggerGB::settings()
{
  return m_gbSettings;
}

void DebuggerGB::continueExecution()
{
  if(isRunning())
    m_net->requestContinue();
}

void DebuggerGB::stop()
{
  if(isRunning())
    m_net->requestStop();
}

void DebuggerGB::stepInto()
{
  if(isRunning())
    m_net->requestStepInto();
}

void DebuggerGB::stepOver()
{
  if(isRunning())
    m_net->requestStepOver();
}

void DebuggerGB::stepOut()
{
  if(isRunning())
    m_net->requestStepOut();
}

void DebuggerGB::addBreakpoints(const QValueList<DebuggerBreakpoint*>&)
{}

void DebuggerGB::addBreakpoint(DebuggerBreakpoint*)
{}

void DebuggerGB::changeBreakpoint(DebuggerBreakpoint*)
{}

void DebuggerGB::removeBreakpoint(DebuggerBreakpoint*)
{}

void DebuggerGB::changeCurrentExecutionPoint(DebuggerExecutionPoint*)
{}

void DebuggerGB::modifyVariable(Variable*, DebuggerExecutionPoint*)
{}


void DebuggerGB::addWatch(const QString& )
{}

void DebuggerGB::removeWatch(const QString& )
{}

void DebuggerGB::profile(const QString&, bool)
{}


void DebuggerGB::slotSettingsChanged()
{
  if(m_gbSettings->enableJIT())
  {
    //do not try to restart if we are already listening on the given port
    if(!m_isJITActive || (m_listenPort != m_gbSettings->listenPort()))
    {
      startJIT();
    }
  }
  else
  {
    stopJIT();
  }
}

void DebuggerGB::slotGBStarted()
{
  m_isRunning = true;
  emit sigDebugStarted(this);
}

void DebuggerGB::slotGBClosed()
{
  m_isRunning = false;
  if(!m_gbSettings->enableJIT())
  {
    stopJIT();
  }

  emit sigDebugEnded();  
}

bool DebuggerGB::startJIT()
{
  if(m_isJITActive)
  {
    stopJIT();
  }

  if(m_net->startListener(m_gbSettings->listenPort()))
  {
    m_isJITActive = true;
    kdDebug() << "Gubed: Listening on port " << m_gbSettings->listenPort() << endl;
    m_listenPort = m_gbSettings->listenPort();
  }
  else
  {
    emit sigInternalError(i18n("Unable to listen on port: %1").arg(
                            m_gbSettings->listenPort()));
    return false;
  }
  return true;
}

void DebuggerGB::stopJIT()
{
  if(m_isRunning)
  {
    m_net->requestStop();
  }

  m_net->stopListener();
  m_isJITActive = false;
}

void DebuggerGB::updateStack(DebuggerStack* stack)
{
//   m_currentExecutionPoint = stack->topExecutionPoint();
//   m_globalExecutionPoint  = stack->bottomExecutionPoint();
  manager()->updateStack(stack);

  emit sigDebugBreak(); 
}

void DebuggerGB::updateVars(const QString& scope, const QString& vars) 
{
  if(scope == "Current Scope") {
    VariableParser p(vars);
    VariablesList_t* array = p.parseAnonymousArray();    
    manager()->updateGlobalVars(array);
  }
}
// void DebuggerGB::requestWatches()
// {
//   if(isRunning())
//   {
//     QValueList<QString>::iterator it;
//     for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it)
//     {
//       //m_net->requestWatch(*it);
//     }
//   }
// }


#include "debuggergb.moc"
