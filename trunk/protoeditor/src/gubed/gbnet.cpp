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

/*  Much of this code is based on Linus McCabe's work on Quanta+ 
*/

#include "gbnet.h"
#include "debuggergb.h"

#include "connection.h"
#include "sitesettings.h"
#include "gbsettings.h"
#include "session.h"

#include <qsocket.h>
#include <kdebug.h>
#include <kurl.h>
#include <qregexp.h>
#include <stdarg.h>

#include "debuggerstack.h"
#include "protoeditorsettings.h"
#include "sitesettings.h"

#define GBD_PROTOCOL_VERSION "0.0.12"

GBNet::GBNet(DebuggerGB* debugger, QObject *parent, const char *name)
    : QObject(parent, name), m_debugger(debugger), m_con(0), m_socket(0), m_datalen(-1)
{
  m_con = new Connection();
  connect(m_con, SIGNAL(sigAccepted(QSocket*)), this, SLOT(slotIncomingConnection(QSocket*)));
  connect(m_con, SIGNAL(sigClientClosed()), this, SLOT(slotGBClosed()));
  connect(m_con, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));
}

GBNet::~GBNet()
{
  delete m_con;
}

bool GBNet::startListener(int port)
{
  return m_con->listenOn(port);
}

void GBNet::stopListener()
{
  m_con->close();
}


void GBNet::startDebugging(const QString& filePath, SiteSettings* site, bool local)
{
  if(local) 
  {
    QString cmd = m_debugger->settings()->startSessionScript() 
                + " " + filePath;
    
    Session::self()->start(cmd, true);
  } 
  else
  {
    QString relative = filePath;
    relative = relative.remove(0, site->localBaseDir().length());

    KURL url = site->effectiveURL();
    url.setPath(m_debugger->settings()->startSessionScript());
    url.setQuery(QString("gbdScript=")+relative);
  
    Session::self()->start(url);
  }
}

void GBNet::requestContinue()
{
  sendCommand("run", 0);
}

void GBNet::requestStop()
{
  sendCommand("die", 0);
  m_con->closeClient();
}

void GBNet::requestStepInto()
{
  sendCommand("next", 0);
}

void GBNet::requestStepOver()
{
  sendCommand("stepover", 0);
}

void GBNet::requestStepOut()
{
  sendCommand("stepout", 0);
}

void GBNet::requestWatches(const QStringList& list)
{
  QStringList::const_iterator it;
  for(it = list.begin(); it != list.end(); it++)
  {
    requestWatch(*it);
  }  
}

void GBNet::requestWatch(const QString& expression)
{
  sendCommand("getwatch", "variable", expression.ascii(), 0);
}

void GBNet::slotIncomingConnection(QSocket* socket)
{
  connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadBuffer()));
  m_socket = socket;

  sendCommand("wait", 0);
//   sendCommand("next");
//   sendCommand("next");

//   //sendCommand("sendbacktrace");
}

void GBNet::slotGBClosed()
{
  emit sigGBClosed();
}

void GBNet::slotError(const QString& msg)
{
  error(msg);
}


void GBNet::slotReadBuffer()
{
  QString buff;
  
  int idx = 0;
  char *data;
  while(m_socket && (m_socket->bytesAvailable() > 0))
  {
    int datalen = m_socket->bytesAvailable();
    data = new char[datalen];
    m_socket->readBlock(data, datalen);
    buff += QString::fromAscii(data, datalen);
    delete data;

    if(buff.find(";") == -1) 
    {
      if(m_socket->bytesAvailable() == 0) 
      {
        m_socket->waitForMore (-1, 0L);
//         kdDebug() << "++++++++++++++++WAITED! " << buff << endl;
        continue;
      }
    }

    while(1) 
    {
//       kdDebug() << "buff: " << buff << endl;

      QRegExp rx;
      rx.setPattern("([^:]*):(\\d*);");  
      rx.search(buff, idx);
  
      int dsize = rx.cap(2).toLong();
      int idx = rx.matchedLength();
      if(buff.length() < (unsigned int)(rx.matchedLength() + dsize)) 
      {
        if(m_socket->bytesAvailable() == 0) 
        {
          m_socket->waitForMore (-1, 0L);
/*          kdDebug() << "---------------------WAITED! " 
            << "dsize: " << dsize << ", idx: " << idx << ", buff.len: " << buff.length()        
            << buff << endl;*/
          break;
        }
      }
  
//   std::cerr << "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n\n\n";
// 
//   std::cerr << "vars: " << buff.mid(idx).ascii() << std::endl;
// 
//   std::cerr << "\n\n----------------------------------------------------\n\n\n\n";


      m_command = rx.cap(1);
      
//       kdDebug() << "command: " << m_command << endl;      
  
      if(dsize) 
      {
        processCommand(buff.mid(idx));
      }
      else 
      {
        processCommand(QString::null);
      }
      buff.remove(0, idx+dsize);
      if(buff.length() == 0)
        break;
    }
  }
}

// Process a gubed command
void GBNet::processCommand(const QString& datas)
{
//   kdDebug() << k_lineinfo << ", received " << m_command << " with data: " << datas << endl;

  StringMap args = parseArgs(datas);

  // See what command we got and act accordingly..
  if(m_command == "commandme")
  {
    sendCommand("sendbacktrace",0);
    sendCommand("sendallvariables",0);
    emit sigStepDone();
  }
  else if(m_command == "initialize")
  {
    emit sigGBStarted();
    sendCommand("havesource", 0);
    requestStepInto();
  }
  else if(m_command == "backtrace") 
  {
    processBacktrace(args["backtrace"]);
  }
  // Send run mode to script
  else if(m_command == "getrunmode")
  {
    sendCommand("pause", 0);
    //sendCommand("run", 0);

    //sendCommand("seterrormask", "errormask", QString::number(m_errormask).ascii(), 0);
  }
  else if(m_command == "variable")
  {
    processVariables(args["variable"]);
  }
  // Show the contents of a watched variable
  else if(m_command == "watch")
  {
    processVariable(args["variable"]);
  }

  // Just some status info, display on status line
  else if(m_command == "status")
  {
/*    long argcnt = args["args"].toLong();
    QString msg = i18n(args["message"]);  // How will we get these messages throught to the translators?
    for(int cnt = 1; cnt <= argcnt; cnt++)
      msg.replace("%" + QString("%1").arg(cnt) + "%", args[QString("arg%1").arg(cnt)]);

    debuggerInterface()->showStatus(msg, false);*/
  }
  // New current line
  else if(m_command == "setactiveline")
  {
//     debuggerInterface()->setActiveLine(mapServerPathToLocal(args["filename"]), args["line"].toLong());
  }
  // Script requests breakpointlist
  else if(m_command == "sendbreakpoints")
  {
//     sendBreakpoints();
  }
  // Parsing failed
  else if(m_command == "parsefailed")
  {
//     emit sigError(i18n("Syntax or parse error in %1)").arg(args["filenme"]));
    return;
  }
  // A debugging session is running
  else if(m_command == "debuggingon")
  {
//     debuggingState(true);
  }
  // No session is running
  else if(m_command == "debuggingoff")
  {
//     debuggingState(false);
  }
  // We stumbled upon an error
  else if(m_command == "error")
  {
    // Put the line number first so double clicking will jump to the corrrect line
/*    debuggerInterface()->showStatus(i18n("Error occurred: Line %1, Code %2 (%3) in  %4").arg(args["line"]).arg(args["errnum"]).arg(args["errmsg"]).arg(args["filename"]), true);

    // Filter to get error code only and match it with out mask
    long error = args["errnum"].toLong();
    if(m_errormask & error)
      setExecutionState(Pause);
    else if(m_executionState == Trace)
      setExecutionState(Trace);
    else if(m_executionState == Run)
      setExecutionState(Run);
    else
      setExecutionState(Pause);

    emit updateStatus(DebuggerUI::HaltedOnError);*/
  }
  // We came across  a hard coded breakpoint
  else if(m_command == "forcebreak")
  {
/*    setExecutionState(Pause);
    emit updateStatus(DebuggerUI::HaltedOnBreakpoint);
    debuggerInterface()->showStatus(i18n("Breakpoint reached"), true);*/
  }
  // A conditional breakpoint was fulfilled
  else if(m_command == "conditionalbreak")
  {
/*    setExecutionState(Pause);
    emit updateStatus(DebuggerUI::HaltedOnBreakpoint);
    debuggerInterface()->showStatus(i18n("Conditional breakpoint fulfilled"), true);*/
  }
  // There is a breakpoint set in this file/line
  else if(m_command == "removebreakpoint")
  {
//     debuggerInterface()->havenoBreakpoint(mapServerPathToLocal(args["filename"]), args["line"].toLong());
  }
  // We're about to debug a file..
  else if(m_command == "sendingwatches")
  {

  }
  // Show the contents of a variable
  else if(m_command == "showcondition")
  {
//     showCondition(args); 
  }
  else if(m_command == "sentwatches")
  {

  }
  // Reached en of an include
  else if(m_command == "end")
  {
    return;
  }
  // Check protocol version
  else if(m_command == "protocolversion")
  {
    if(args["version"] != GBD_PROTOCOL_VERSION)
    {
//       debuggerInterface()->showStatus(i18n("The script being debugged does not communicate with the correct protocol version"), true);
//       sendCommand("die", 0);
    }
    return;
  }
  // Instructions we currently ignore
  else if(m_command == "sourcesent"
          || m_command == "addsourceline"
         )
  {}
  else
    // Unimplemented command - log to debug output
    kdDebug() << "Gubed: unknown/unsupported " << m_command << ":" << datas << endl;
}


StringMap GBNet::parseArgs(const QString &args)
{
  StringMap ca;
  long cnt, length;

  // a:2:{s:4:"name";s:7:"Jessica";s:3:"age";s:2:"26";s:4:"test";i:1;}

  // No args
  if(args.isEmpty() || args == "a:0:{}")
    return ca;

  // Make sure we have a good string
  if(!args.startsWith("a:"))
  {
    kdDebug() << k_funcinfo << "An error occurred in the communication link, data received was:" << args << endl;
    return ca;
  }

  cnt = args.mid(2, args.find("{") - 3).toLong();
  QString data = args.mid(args.find("{") + 1);

  QString tmp, func;
  while(cnt > 0)
  {
    tmp = data.left(data.find("\""));
    length = tmp.mid(2, tmp.length() - 3).toLong();

    func = data.mid(tmp.length() + 1, length);
    data = data.mid( tmp.length() + length + 3);

    if(data.left(1) == "i")
    {
      // Integer data
      tmp = data.mid(data.find(":") + 1);
      tmp = tmp.left(tmp.find(";"));
      ca[func] = tmp;
      data = data.mid(tmp.length() + 3);
    }
    else
    {
      // String data
      tmp = data.left(data.find("\""));
      length = tmp.mid(2, tmp.length() - 3).toLong();

      ca[func] = data.mid(tmp.length() + 1, length);
      data = data.mid( tmp.length() + length + 3);
   }
 
    cnt--;
  }

  return ca;
}

void GBNet::error(const QString& msg)
{
  emit sigError(msg);
  m_con->closeClient();
}

bool GBNet::sendCommand(const QString& command, QMap<QString, QString> args)
{

//   kdDebug() << k_lineinfo << ", command " << command << " with data: " << phpSerialize(args) << endl;

//   if(!m_socket || m_socket->state() != KNetwork::KClientSocketBase::Connected)
//     return false;

  QString buffer = phpSerialize(args);

  buffer = QString(command + ":%1;" + buffer).arg(buffer.length());
  m_socket->writeBlock(buffer, buffer.length());
  return true;
}

bool GBNet::sendCommand(const QString& command, char * firstarg, ...)
{
  StringMap ca;
  char *next;

  va_list l_Arg;
  va_start(l_Arg, firstarg);

  next = firstarg;
  while(next)
  {
    ca[(QString)next] = (QString)va_arg(l_Arg, char*) ; 
    next = va_arg(l_Arg, char*);
  }

  va_end(l_Arg);
  sendCommand(command, ca);
  return true;
}


QString GBNet::phpSerialize(QMap<QString, QString> args)
{
  StringMap::Iterator it;
  // a:2:{s:4:"name";s:7:"Jessica";s:3:"age";s:2:"26";s:4:"test";i:1;}
  QString ret = QString("a:%1:{").arg(args.size());
  for( it = args.begin(); it != args.end(); ++it )
  {
    bool isNumber;

    it.data().toInt(&isNumber);
    if(isNumber && !it.data().isEmpty())
      ret += QString("s:%1:\"%2\";i:%3;")
                    .arg(it.key().length())
                    .arg(it.key())
                    .arg(it.data());
    else
      ret += QString("s:%1:\"%2\";s:%3:\"%4\";")
                    .arg(it.key().length())
                    .arg(it.key())
                    .arg(it.data().length())
                    .arg(it.data());
  }

  ret += "}";
  return ret;
}

void GBNet::processBacktrace(const QString& bt)
{
//   kdDebug() << "bt: " << bt << endl;

  DebuggerStack* stack = new DebuggerStack();

  int idx = 0;

  QString file;
  QString func;
  int line;

  int levels;

  QRegExp rx;

  QString where;
  QString localFile;
  SiteSettings* site = ProtoeditorSettings::self()->currentSiteSettings();

  while(1) 
  {
    rx.setPattern("i:(\\d*);");  
    if(rx.search(bt, idx) == -1) {
      break;
    }
  
    idx += rx.matchedLength();
  
    levels = rx.cap(1).toInt();


    rx.setPattern("a:4:\\{s:\\d*:\"file\";s:\\d*:\"([^\"]*)\";s:\\d*:\"class\";s:\\d*:\"[^\"]*\";s:\\d*:\"function\";s:\\d*:\"([^\"]*)\";s:\\d*:\"line\";i:(\\d*);\\}");
    if(rx.search(bt, idx) == -1) {
      //error!
      return;
    }
  
    idx += rx.matchedLength();
  
    file     = rx.cap(1);
    func     = rx.cap(2);
    line     = rx.cap(3).toInt();
    
    if(func.isEmpty())
    {
      where = file + "::main()";
    }
    else
    {
      where = file + "::" + func + "()";
    }
    
    if(site) 
    {
      localFile = site->localBaseDir()
                          + file.remove(0, site->remoteBaseDir().length());
    }
    else
    {
      localFile = file;
    }

    stack->insert(levels, localFile, line, where);
  }

  m_debugger->updateStack(stack);
}

void GBNet::processVariable(const QString& var)
{
  QRegExp rx;
  rx.setPattern("s:\\d*:\"([^;]*)\";(.*)");  
  if(rx.search(var, 0) == -1)
  {
    //error!
    return;
  }

//   kdDebug() << var << endl;
  m_debugger->updateWatch(rx.cap(1), rx.cap(2));
}

void GBNet::processVariables(const QString& vars)
{
  QRegExp rx;
  rx.setPattern("s:\\d*:\"([^;]*)\";");  
  if(rx.search(vars, 0) == -1) {
    //error!
    return;
  }

  int idx = rx.matchedLength();

  m_debugger->updateVars(rx.cap(1), vars.mid(idx));
}

#include "gbnet.moc"
