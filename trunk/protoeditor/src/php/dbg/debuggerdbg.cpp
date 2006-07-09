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

#include "debuggerdbg.h"
#include "protoeditor.h"
#include "datacontroller.h"
#include "dbgnet.h"
#include "dbg_defs.h"
#include "phpvariableparser.h"
#include "phpvariable.h"
#include "debuggerstack.h"
#include "debuggerbreakpoint.h"
#include "phpdefs.h"
#include "dbgprofiledialog.h"
#include "dbgprofilelistview.h"
#include "dbgprofiledata.h"

#include "protoeditorsettings.h"
#include "dbgsettings.h"
#include "languagesettings.h"

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>


#include <qregexp.h>

DebuggerDBG::DebuggerDBG(LanguageSettings* langs)
    : AbstractDebugger(langs), m_name("dbg"), m_isJITActive(false), m_isRunning(false),
    m_listenPort(-1), m_dbgSettings(0), m_net(0), m_profileDialog(0),
    m_currentExecutionPointID(CURLOC_SCOPE_ID), m_globalExecutionPointID(GLOBAL_SCOPE_ID)
{
  
  m_dbgSettings = new DBGSettings(name(), label(), langSettings());
  langSettings()->registerDebuggerSettings(name(), m_dbgSettings);

  connect(Protoeditor::self()->settings(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  m_net = new DBGNet(this);
  m_net->setOptions(getDBGOptions());

  connect(m_net, SIGNAL(sigDBGStarted()), this, SLOT(slotDBGStarted()));
  connect(m_net, SIGNAL(sigDBGClosed()), this, SLOT(slotDBGClosed()));
  connect(m_net, SIGNAL(sigError(const QString&)), this, SIGNAL(sigInternalError(const QString&)));
  connect(m_net, SIGNAL(sigStepDone()), this, SLOT(slotStepDone()));
  connect(m_net, SIGNAL(sigBreakpoint()), this, SLOT(slotBreakpoint()));
  connect(m_net, SIGNAL(sigNewConnection()), this, SLOT(slotNewConnection()));
}

DebuggerDBG::~DebuggerDBG()
{
  delete m_net;
  delete m_dbgSettings;
  delete m_profileDialog;
}

void DebuggerDBG::init()
{
  slotSettingsChanged();
}

QString DebuggerDBG::name() const
{
  return m_name;
}

QString DebuggerDBG::label()   const
{
  return i18n("DBG");
}

bool DebuggerDBG::isRunning() const
{
  return m_isRunning;
}

void DebuggerDBG::slotSettingsChanged()
{
  if(m_dbgSettings->languageSettings()->isEnabled() && m_dbgSettings->enableJIT())
  {
    //do not try to restart if we are already listening on the given port
    if(!m_isJITActive || (m_listenPort != m_dbgSettings->listenPort()))
    {
      startJIT();
    }
  }
  else
  {
    stopJIT();
  }

  m_net->setOptions(getDBGOptions());
}

bool  DebuggerDBG::startJIT()
{
  if(m_isJITActive)
  {
    stopJIT();
  }

  if(m_net->startListener(m_dbgSettings->listenPort()))
  {
    m_isJITActive = true;
    kdDebug() << "DBG: Listening on port " << m_dbgSettings->listenPort() << endl;
    m_listenPort = m_dbgSettings->listenPort();
  }
  else
  {
    emit sigInternalError(i18n("Unable to listen on port: %1").arg(
                            m_dbgSettings->listenPort()));
    return false;
  }
  return true;
}

void DebuggerDBG::stopJIT()
{
  if(m_isRunning)
  {
    m_net->requestStop();
  }

  m_net->stopListener();
  m_isJITActive = false;
}

void DebuggerDBG::start(const QString& filePath, const QString& args, bool local)
{
  SiteSettings* site  = Protoeditor::self()->settings()->currentSiteSettings();

  dbgint sessionid = kapp->random();

  if(!m_isJITActive && !startJIT())
  {
    return;
  }

  emit sigDebugStarting();
  
  m_net->startDebugging(filePath, args, site, local, m_dbgSettings->listenPort(), sessionid);
}

void DebuggerDBG::continueExecution()
{
  if(m_isRunning)
  {
    m_net->requestContinue();
  }
}

int DebuggerDBG::getDBGOptions()
{
  int opts = 0;
  if(m_dbgSettings->breakOnLoad())
  {
    opts |= SOF_BREAKONLOAD;
  }
  if(m_dbgSettings->sendDetailedOutput())
  {
    opts |= SOF_SEND_OUTPUT_DETAILED;
  }

  if(m_dbgSettings->sendErrors())
  {
    opts |= SOF_SEND_ERRORS;
  }

  if(m_dbgSettings->sendLogs())
  {
    opts |= SOF_SEND_LOGS;
  }

  if(m_dbgSettings->sendOutput())
  {
    opts |= SOF_SEND_OUTPUT;
  }
  return opts;
}

void DebuggerDBG::stop()
{
  if(isRunning())
  {
    m_net->requestStop();
  }
}

void DebuggerDBG::runToCursor(const QString& filePath, int line)
{
  if(isRunning())
  {
    m_net->requestRunToCursor(filePath, line);
    continueExecution();
  }
}

void DebuggerDBG::stepInto()
{
  if(isRunning())
  {
    m_net->requestStepInto();
  }
}

void DebuggerDBG::stepOver()
{
  if(isRunning())
  {
    m_net->requestStepOver();
  }
}

void DebuggerDBG::stepOut()
{
  if(isRunning())
  {
    m_net->requestStepOut();
  }
}

void DebuggerDBG::addBreakpoints(const QValueList<DebuggerBreakpoint*>& bps)
{
  if(isRunning())
  {
    QValueList<DebuggerBreakpoint*>::const_iterator it;
    for(it = bps.begin(); it != bps.end(); ++it)
    {
      m_net->requestBreakpoint(*it);
    }
  }
}

void DebuggerDBG::addBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpoint(bp);
  }
}

/*
void DebuggerDBG::addBreakpoint(const QString&, int)
{}
*/

void DebuggerDBG::changeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpoint(bp);
  }
}

void DebuggerDBG::removeBreakpoint(DebuggerBreakpoint* bp)
{
  if(isRunning())
  {
    m_net->requestBreakpointRemoval(bp->id());
  }
}


void DebuggerDBG::modifyVariable(Variable* var, DebuggerExecutionPoint* execPoint)
{
  if(isRunning())
  {
    QString name  =  var->compositeName();
    QString value =  var->value()->toString();

    if(value.isEmpty()) value = "null";

    m_net->requestWatch(name + "=" + value, execPoint->id());

    //reload variables (global/local/watches) to get the new value.

    m_net->requestVariables(m_globalExecutionPointID, true);
    m_net->requestVariables(m_currentExecutionPointID, false);
    requestWatches(m_currentExecutionPointID);
  }
}

void DebuggerDBG::changeCurrentExecutionPoint(DebuggerExecutionPoint* execPoint)
{
  m_currentExecutionPointID = execPoint->id();
  if(isRunning())
  {
    m_net->requestVariables(execPoint->id(), false);
  }
}


void DebuggerDBG::addWatches(const QStringList& list)
{
  m_wathcesList = list;  

  if(isRunning())
  {
    QStringList::iterator it;
    for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it)
    {
      m_net->requestWatch(*it, m_currentExecutionPointID);
    }
  }
}

void DebuggerDBG::addWatch(const QString& expression)
{
  if(m_wathcesList.find(expression) == m_wathcesList.end())
  {
    m_wathcesList.append(expression);
  }

  if(isRunning())
  {
    m_net->requestWatch(expression, m_currentExecutionPointID);
  }
}

void DebuggerDBG::removeWatch(const QString& expression)
{
  QStringList::iterator it = m_wathcesList.find(expression);

  if(it != m_wathcesList.end())
  {
    m_wathcesList.remove(it);
  }
}

void DebuggerDBG::profile(const QString& filePath, const QString& args, bool local)
{
  profileDialog()->clear();

  SiteSettings* site  = Protoeditor::self()->settings()->currentSiteSettings();

  dbgint sessionid = kapp->random();

  if(!m_isJITActive && !startJIT())
  {
    return;
  }

  emit sigDebugStarting();
  
  m_net->startProfiling(filePath, args, site, local, m_dbgSettings->listenPort(), sessionid);
}

DBGProfileDialog* DebuggerDBG::profileDialog()
{
  if(!m_profileDialog)
  {
    m_profileDialog = new DBGProfileDialog(0, "profile");
    
    connect(m_profileDialog->listview(), SIGNAL(sigDoubleClick(const QString&, int)),
            Protoeditor::self(), SLOT(slotGotoLineAtFile(const QString&, int )));
  }

  return m_profileDialog;
}

void DebuggerDBG::requestWatches(int scopeid)
{
  if(isRunning())
  {
    QStringList::iterator it;
    for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it)
    {
      m_net->requestWatch(*it, scopeid);
    }
  }
}

/**************************** DBGNET **************************/

void DebuggerDBG::updateStack(DebuggerStack* stack)
{
  m_currentExecutionPointID = stack->topExecutionPoint()->id();
  m_globalExecutionPointID  = stack->bottomExecutionPoint()->id();
  Protoeditor::self()->dataController()->updateStack(stack);
}

void DebuggerDBG::updateVar(const QString& result, const QString& str, bool isGlobal)
{
  if(str.isEmpty())
  {
    //global vars
    PHPVariableParser p(result);

    VariableList_t* array = p.parseAnonymousArray();

    if(isGlobal)
    {
      Protoeditor::self()->dataController()->updateGlobalVars(array);
    }
    else
    {
      Protoeditor::self()->dataController()->updateLocalVars(array);
    }
  }
}

void DebuggerDBG::updateWatch(const QString& result, const QString& str)
{
  if(m_wathcesList.find(str) == m_wathcesList.end())
  {
    //This watch is not on our list.
    //It might happen whe the user modifies the value of a variable (ie. "$var=123").
    //Since we request a watch expression "$var=123", we receive "$var=123" as the name
    //of the variable (str) and we don't want to add something like that to the watchlist
    //everytime the user modifies a variable, right? :)
    return;
  }

  PHPVariable* var;
  if(!result.isEmpty())
  {
    PHPVariableParser p(result);

    var = p.parseVariable();
    var->setName(str);
  }
  else
  {
    var = new PHPVariable(str);
    PHPScalarValue* value = new PHPScalarValue(var);
    var->setValue(value);
  }

  Protoeditor::self()->dataController()->updateWatch(var);
}

void DebuggerDBG::updateBreakpoint(int id, const QString& filePath, int line, int state, int hitcount, int skiphits,
                                   const QString& condition)
{
  int status;
  switch(state)
  {
      //case BPS_DELETED
    case BPS_DISABLED:
      status = DebuggerBreakpoint::DISABLED;
      break;
    case BPS_ENABLED:
      status = DebuggerBreakpoint::ENABLED;
      break;
    case BPS_UNRESOLVED:
    default:
      status = DebuggerBreakpoint::UNRESOLVED;
      break;
  }
  DebuggerBreakpoint* bp = new DebuggerBreakpoint(id, KURL::fromPathOrURL(filePath), 
      line, status, condition, hitcount, skiphits);

  Protoeditor::self()->dataController()->updateBreakpoint(bp);
}

void DebuggerDBG::addProfileData(int modid, const QString& filePath, int ctxid, const QString ctxname, int line, long hitcount, double min, double max, double sum)
{
  if(!profileDialog()->isVisible())
  {
    profileDialog()->show();
  }
  
  m_profileDialog->addData(
    new DBGProfileData(modid, filePath, ctxid, ctxname, line, hitcount, sum / hitcount, sum, min, max));
}

void DebuggerDBG::debugError(/*int type,*/ const QString&)
{
  //NOTE: When a PHP error ocurr, I'm not sure why DBG some times
  //sends a TagError and some times not. So, we are sending errors through the Log
  //Protoeditor::self()->dataController()->debugError(msg);
}

void DebuggerDBG::debugLog(int type, const QString& msg, int line, const QString& filePath, int extInfo)
{
  QString message = msg;
  QRegExp rx;
  switch(type)
  {
    case LT_ODS:
      kdDebug() << msg << endl;
      break;
    case LT_ERROR:
      //remove HTML tag
      rx.setPattern("\\[[^\\]]*\\]");
      message.remove(rx);

      switch(extInfo)
      {
        case E_ERROR:
        case E_CORE_ERROR:
        case E_PARSE:
        case E_COMPILE_ERROR:
        case E_USER_ERROR:
          Protoeditor::self()->dataController()->debugMessage(DataController::ErrorMsg, message, KURL::fromPathOrURL(filePath), line);
          break;

        case E_WARNING:
        case E_CORE_WARNING:
        case E_COMPILE_WARNING:
        case E_USER_WARNING:
          Protoeditor::self()->dataController()->debugMessage(DataController::WarningMsg, message, KURL::fromPathOrURL(filePath), line);
          break;
        case E_NOTICE:
        case E_USER_NOTICE:
        case E_STRICT:
          Protoeditor::self()->dataController()->debugMessage(DataController::InfoMsg, message, KURL::fromPathOrURL(filePath), line);
          break;
      }
      break;
    case LT_OUTPUT:
//       m_output += msg;
      Protoeditor::self()->dataController()->addOutput(msg);
      break;
    case LT_FATALERROR:
      break;
  }
}

void DebuggerDBG::checkDBGVersion(int major, int minor, const QString& desc)
{
  kdDebug() << desc << endl;

  if((major != DBG_API_MAJOR_VESION) ||
      (minor != DBG_API_MINOR_VESION))
  {

/*    emit sigInternalError(QString("Incompatible DBG version. Expecting %1.%2.").arg(
                            QString::number(DBG_API_MAJOR_VESION), QString::number(DBG_API_MINOR_VESION)));*/
  }
}

/***************************** SLOTS *********************************/


void DebuggerDBG::slotDBGStarted()
{
  m_firstStep = true;
  m_isRunning = true;
  emit sigDebugStarted(this);
}

void DebuggerDBG::slotDBGClosed()
{
  //end of debug

  m_isRunning = false;
//   m_output = QString::null;

  if(!m_dbgSettings->enableJIT())
  {
    stopJIT();
  }

  emit sigDebugEnded();
}

void DebuggerDBG::processStepData()
{
  m_currentExecutionPointID = CURLOC_SCOPE_ID;
  m_net->requestVariables(m_currentExecutionPointID, false);
  m_net->requestVariables(m_globalExecutionPointID, true);
  requestWatches(m_currentExecutionPointID);
  //   requestProfileData();
}

// void DebuggerDBG::requestProfileData()
// {
//   if(isRunning())
//   {
//     if(m_isProfilingEnabled)
//     {
//       profileDialog()->clear();
//       m_net->profile();
//     }
//   }
// }

void DebuggerDBG::slotBreakpoint()
{
  processStepData();
  emit sigDebugPaused();
}

void DebuggerDBG::slotStepDone()
{
  processStepData();

  //hide the first step. Its done automatically on DBGNet::processHeader() - DBGC_STARTUP.
  if(!m_firstStep)
  {
    emit sigDebugPaused();
  }
  else
  {
    m_firstStep = false;
  }
}

void DebuggerDBG::slotNewConnection()
{
  //we don't know if it is requested session or JIT, so we have
  //to update the m_net site
  m_net->setSite(Protoeditor::self()->settings()->currentSiteSettings());

  //tell everyone to cleanup any pending session
  emit sigJITStarted(this);
}

#include "debuggerdbg.moc"
