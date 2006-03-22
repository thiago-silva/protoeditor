/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
 *   thiago.silva@kdemail.net                                              *
 *   Copied/Based on Linus McCabe work on Quanta                           * 
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


#include "gbnet.h"
#include "debuggergb.h"

#include "phpdefs.h"
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
#include "debuggerbreakpoint.h"
#include "protoeditorsettings.h"
#include "sitesettings.h"

#define GBD_PROTOCOL_VERSION "0.0.12"

GBNet::GBNet(DebuggerGB* debugger, QObject *parent, const char *name)
    : QObject(parent, name), m_debugger(debugger), m_con(0), m_socket(0), m_site(0)
    , m_watchingGlobal(false), m_continuing(false)
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


void GBNet::startDebugging(const QString& filePath, const QString& uiargs,
  SiteSettings* site, bool local)
{
  if(local)
  {
    QString args = filePath;
    if(!uiargs.isEmpty())
    {
      args += " " + uiargs;
    }
    Session::self()->start(m_debugger->settings()->startSessionScript(), args);
  }
  else
  {
    QString relative = filePath;
    relative = relative.remove(0, site->localBaseDir().length());

    KURL url = site->effectiveURL();
    url.setPath(m_debugger->settings()->startSessionScript());

    QString query = "gbdScript=";
    query += relative;

    if(!uiargs.isEmpty())
    {
      query += "&" + uiargs;
    }
    url.setQuery(query); 
  
    Session::self()->start(url);
  }
}

void GBNet::requestContinue()
{
  sendCommand("next", 0);
  sendCommand("run", 0);
  m_continuing = true;
}

void GBNet::requestStop()
{
  sendCommand("die", 0);
  m_socket->flush();
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

void GBNet::requestGlobals()
{
  sendCommand("sendallvariables",0);
}

void GBNet::requestWatches(const QStringList& list)
{
  m_watchingGlobal = false;

  QStringList::const_iterator it;
  for(it = list.begin(); it != list.end(); it++)
  {
    if(*it == "$GLOBALS") 
    {
      m_watchingGlobal = true;
    }
    requestWatch(*it);
  }  
}

void GBNet::requestWatch(const QString& expression)
{
  sendCommand("getwatch", "variable", expression.ascii(), 0);
}

void GBNet::requestChangeVar(const QString& name, const QString& value)
{
  sendCommand("setvariable", "variable", name.ascii(), "value", value.ascii(), 0);
}

void GBNet::slotIncomingConnection(QSocket* socket)
{
  connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadBuffer()));
  m_socket = socket;

  m_site = ProtoeditorSettings::self()->currentSiteSettings();
  sendCommand("wait", 0);
}


void GBNet::requestBreakpoint(DebuggerBreakpoint* bp)
{
  QString filePath = bp->url().path();
  if(m_site) 
  {    
    filePath = m_site->remoteBaseDir() + filePath.remove(0, m_site->localBaseDir().length());
  }

  sendCommand("breakpoint", 
              "type", (bp->condition().isEmpty())?"line":"true",
              "filename", filePath.ascii(),
              "expression", bp->condition().ascii(),
              "line", QString::number(bp->line()-1).ascii(),
              0);
}

void GBNet::requestBreakpointUpdate(DebuggerBreakpoint* bp)
{
  QString filePath = bp->url().path();
  if(m_site) 
  {    
    filePath = m_site->remoteBaseDir() + filePath.remove(0, m_site->localBaseDir().length());
  }

//   sendCommand("breakpoint", 
//               "type", "change",
//               "filename", filePath.ascii(),
//               "expression", bp->condition().ascii(),
//               "line", QString::number(bp->line()-1).ascii(),
//               0);
}

void GBNet::requestBreakpointRemoval(DebuggerBreakpoint* bp)
{
  QString filePath = bp->url().path();
  if(m_site) 
  {    
    filePath = m_site->remoteBaseDir() + filePath.remove(0, m_site->localBaseDir().length());
  }

  sendCommand("removebreakpoint",
              "type", (bp->condition().isEmpty())?"line":"true",
              "filename", filePath.ascii(),
              "expression", bp->condition().ascii(),
              "line", QString::number(bp->line()-1).ascii(),
              0);
}

void GBNet::slotGBClosed()
{
  m_site = NULL;
  m_continuing = false;
  m_watchingGlobal = false;
  m_socket = NULL;
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
          break;
        }
      }
  
      m_command = rx.cap(1);
  
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
//   kdDebug() << "gbd command " << m_command << "\n";//" with data: " << datas << endl;

  StringMap args = parseArgs(datas);

  // See what command we got and act accordingly..
  if(m_command == "commandme")
  {
    if(!m_continuing)
    {
      sendCommand("sendbacktrace",0);
      emit sigStepDone();
    }
    else
    {
      sendCommand("next",0);
    }
  }
  else if(m_command == "forcebreak")
  {
    m_continuing = false;
    //sendCommand("pause",0);
  }
  else if(m_command == "initialize")
  {    
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
    sendCommand("seterrormask", "errormask", QString::number(E_ALL).ascii(), 0);

    emit sigGBStarted();
    if(!m_debugger->settings()->breakOnLoad())
    {
      sendCommand("sendbacktrace",0);
      requestContinue();
    }    
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
  else if(m_command == "error")
  {
//     kdDebug() << "error data " << datas << endl;
    QString msg = args["errmsg"];
    QString filename = args["filename"];
    QString line = args["line"];

    processLog(datas);

    if(m_continuing)
    {
      requestContinue();
    }
    else
    {
      sendCommand("pause",0);
      requestStepInto();
      emit sigStepDone();
    }
  }
  // Just some status info, display on status line
  else if(m_command == "status")
  {
//     kdDebug() << "status: " << datas << endl;

    /*long argcnt = args["args"].toLong();
    QString msg = i18n(args["message"]);  // How will we get these messages throught to the translators?
    for(int cnt = 1; cnt <= argcnt; cnt++)
      msg.replace("%" + QString("%1").arg(cnt) + "%", args[QString("arg%1").arg(cnt)]);

    debuggerInterface()->showStatus(msg, false);*/
  }
  // Script requests breakpointlist
  else if(m_command == "sendbreakpoints")
  {

  }
  // Parsing failed
  else if(m_command == "parsefailed")
  {    
    processFatalError(datas);
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
  // We came across  a hard coded breakpoint

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
  // New current line
  else if(m_command == "setactiveline")
  {

  }  // Reached en of an include
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

  while(1) 
  {
    rx.setPattern("i:(\\d*);");
    if(rx.search(bt, idx) == -1) break;
  
    idx += rx.matchedLength();
  
    levels = rx.cap(1).toInt();

    rx.setPattern("a:4:\\{s:\\d*:\"file\";s:\\d*:\"([^\"]*)\";s:\\d*:\"class\";s:\\d*:\"[^\"]*\";s:\\d*:\"function\";s:\\d*:\"([^\"]*)\";s:\\d*:\"line\";i:(\\d*);\\}");
    if(rx.search(bt, idx) == -1) {
      error("Error receiving network data.");
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
      where = func + "()";
    }
    
    if(m_site)
    {
      localFile = m_site->localBaseDir()
                          + file.remove(0, m_site->remoteBaseDir().length());
    }
    else
    {
      localFile = file;
    }

    stack->insert(levels, KURL::fromPathOrURL(localFile), line, where);
  }

  m_debugger->updateStack(stack);
}

void GBNet::processVariable(const QString& var)
{
  QRegExp rx;
  rx.setPattern("s:\\d*:\"([^;]*)\";(.*)");  
  if(rx.search(var, 0) == -1)
  {
    error("Error receiving network data.");
    return;
  }

  QString name = rx.cap(1);
 
  if(name == "$GLOBALS")
  {
    if(m_watchingGlobal)
    {
      m_debugger->updateWatch(name, rx.cap(2));
    }
    m_debugger->updateGlobalVariables(rx.cap(2));
  }
  else
  {
    m_debugger->updateWatch(name, rx.cap(2));
  }
}

void GBNet::processVariables(const QString& vars)
{
  QRegExp rx;
  rx.setPattern("s:\\d*:\"([^;]*)\";");  
  if(rx.search(vars, 0) == -1) {
    error("Error receiving network data.");
    return;
  }

  int idx = rx.matchedLength();

  m_debugger->updateLocalVariables(rx.cap(1), vars.mid(idx));
}


void GBNet::processLog(const QString& log)
{
  QRegExp rx;
  rx.setPattern("a:4:\\{s:\\d*:\"filename\";s:\\d*:\"([^;]*)\";s:\\d*:\"line\";i:(\\d*);s:\\d*:\"errnum\";i:(\\d*);s:\\d*:\"errmsg\";s:\\d*:\"([^;]*)\";");
  if(rx.search(log, 0) == -1) {
    error("Error receiving network data.");
    return;
  }

  QString filePath = rx.cap(1);
  filePath = m_site->localBaseDir() + filePath.remove(0, m_site->remoteBaseDir().length());

  m_debugger->updateMessage(rx.cap(3).toInt(), rx.cap(4), filePath, rx.cap(2).toInt());
}


void GBNet::processFatalError(const QString& err)
{
  //parsefailed : a:1:{s:8:"filename";s:40:"/usr/local/apache-1.3.34/htdocs/test.php";}
  QRegExp rx;
  rx.setPattern("a:1:\\{s:\\d*:\"filename\";s:\\d*:\"([^;]*)\";\\}");

  if(rx.search(err, 0) == -1) 
  {
    error("Error receiving network data.");
    return;
  }

  QString filePath = m_site->localBaseDir() + rx.cap(1).remove(0, m_site->remoteBaseDir().length());

  m_debugger->updateError(filePath);  
}

#include "gbnet.moc"

