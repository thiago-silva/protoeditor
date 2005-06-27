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


DebuggerGB::DebuggerGB(DebuggerManager* manager)
    : AbstractDebugger(manager), m_name("Gubed"), m_isRunning(false), m_isJITActive(false),
    m_gbSettings(0)
{
  m_gbSettings = new GBSettings(m_name);

  ProtoeditorSettings::self()->registerDebuggerSettings(m_gbSettings, m_name);

  connect(ProtoeditorSettings::self(), SIGNAL(sigSettingsChanged()),
          this, SLOT(slotSettingsChanged()));

  m_net = new GBNet(this);
  connect(m_net, SIGNAL(sigGBStarted()), this, SLOT(slotGBStarted()));
  connect(m_net, SIGNAL(sigGBClosed()), this, SLOT(slotGBClosed()));
  connect(m_net, SIGNAL(sigError(const QString&)), this, SLOT(slotInternalError(const QString&)));
  //   connect(m_net, SIGNAL(sigStepDone()), this, SLOT(slotStepDone()));
}


DebuggerGB::~DebuggerGB()
{}

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


void DebuggerGB::run(const QString& filepath)
{
  SiteSettings* site  = ProtoeditorSettings::self()->currentSiteSettings();

  if(!m_isJITActive && !startJIT())
  {
    return;
  }

  emit sigDebugStarting();

  m_net->startDebugging(filepath, site);
}

GBSettings* DebuggerGB::settings()
{
  return m_gbSettings;
}

void DebuggerGB::continueExecution()
{
  m_net->requestContinue();
}


void DebuggerGB::stop()
{
  m_net->requestStop();
}

void DebuggerGB::stepInto()
{
  m_net->requestStepInto();
}

void DebuggerGB::stepOver()
{
  m_net->requestStepOver();
}

void DebuggerGB::stepOut()
{
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

void DebuggerGB::profile(const QString&)
{}


void DebuggerGB::slotSettingsChanged()
{
  if(m_gbSettings->enableJIT())
  {
    startJIT();
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

void DebuggerGB::slotInternalError(const QString& msg)
{
  emit sigInternalError(msg);
}

bool DebuggerGB::startJIT()
{
  if(!m_isJITActive)
  {

    if(m_net->startListener(m_gbSettings->listenPort()))
    {
      m_isJITActive = true;
      kdDebug() << "Gubed: Listening on port " << m_gbSettings->listenPort() << endl;
    }
    else
    {
      emit sigInternalError(i18n("Unable to listen on port: %1").arg(
                              m_gbSettings->listenPort()));
      return false;
    }
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

void DebuggerGB::processInput(const QString& cmd, const QMap<QString, QString>& serialData)
{
  if(cmd == "sendbreakpoints")
  {
    //will be commanded from DebuggerManager when we emit sigDebugStarted
  }
  else if (cmd == "getrunmode")
  {
    m_net->sendRunMode(/*m_gbSettings->executionMode()*/ 0);

   //send hardcoded default error handling
    m_net->sendErrorSettings(/*m_gbSettings->errorSettings()*/ E_WARNING  &  E_USER_ERROR &  E_USER_WARNING);
  }
  else if(cmd == "initialize")
  {
    //skipping checksum
    //skipping protocolversion check : m_net->sendCommand("sendprotocolversion");

    //assuminig we have source
    m_net->sendHaveSource(true, serialData["filename"]);
  }
  else if(cmd == "commandme")
  {
    QString file = serialData["filename"];
    QString line = serialData["line"];
    
    /*
    m_currentExecutionPoint->setLine(serialData["line"].toInt());

    //TODO! missing translation
    m_currentExecutionPoint->setFilePath(serialData["filename"].toInt());
    */
    
    requestWatches();
    
    m_net->requestBacktrace();

    /*
    if(m_gbSettings->executionMode() == GBSettings::PauseMode)
    {
      m_net->sendWait();
    }

    if(m_gbSettings->executionMode() == GBSettings::Trace)
    {
      m_net->sendNext();
    }*/
    m_net->sendWait();
  } else if(cmd == "backtrace")
  {
    /*
    DebuggerStack* st = new DebuggerStack();
    
    QString bt = serialData["backtrace"];

    int frame;
    while(bt.Length() > 0)
    {
      QString tmp, arg;
      tmp = bt.mid(0, bt.find(';'));
      frame = tmp.AfterFirst(':'));
    
      bt = bt.AfterFirst('{');
     
      while(bt.Mid(0, 1) != wxT("}") && bt.Length() > 0)
      {
        int len = wxAtoi(bt.AfterFirst(':').BeforeFirst(':'));
        arg = bt.AfterFirst('"').Mid(0, len);
        bt = bt.AfterFirst(';');
      
        tmp = wxT("");
        if(bt.Mid(0, 1) == wxT("s"))
        {
          int len = wxAtoi(bt.AfterFirst(':').BeforeFirst(':'));
          bt = bt.AfterFirst('"');
          tmp = bt.Mid(0, len);
          bt = bt.Mid(len + 2);
        }
        else
        {
          tmp = bt.BeforeFirst(';').AfterFirst(':');
          bt = bt.AfterFirst(';');
        }
    
        if(arg == wxT("function"))
          m_backtracelist->SetCellValue(frame, BacktraceColumns::Function, tmp + wxT(" "));
        else if(arg == wxT("class"))
          m_backtracelist->SetCellValue(frame, BacktraceColumns::Class, tmp + wxT(" "));
        else if(arg == wxT("file"))
        {
          m_backtracelist->SetCellValue(frame, BacktraceColumns::Filename, tmp + wxT(" "));
          m_backtracelist->SetCellAlignment(wxALIGN_RIGHT, frame, BacktraceColumns::Filename);
        }
        else if(arg == wxT("line"))
        {
          m_backtracelist->SetCellValue(frame, BacktraceColumns::Line, tmp + wxT(" "));
          m_backtracelist->SetCellAlignment(wxALIGN_RIGHT, frame, BacktraceColumns::Line);
        }
      }
    }
      
  //  i:0;a:4:{s:4:"file";s:82:"classes/baseclass.php";s:5:"class";s:9:"baseclass";s:8:"function";s:5:"func1";s:4:"line";i:16;}i:1;a:4:{s:4:"file";s:77:"/classes/...
  
  //wxString asdf   
  
    m_backtracelist->EndBatch();
    */
  }
}

void DebuggerGB::requestWatches()
{
  if(isRunning())
  {
    QValueList<QString>::iterator it;
    for(it = m_wathcesList.begin(); it != m_wathcesList.end(); ++it)
    {
      //m_net->requestWatch(*it);
    }
  }
}

#include "debuggergb.moc"
