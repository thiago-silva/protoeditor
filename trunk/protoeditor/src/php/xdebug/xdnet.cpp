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

#include "xdnet.h"
#include "debuggerxd.h"
#include "xdvariableparser.h"
#include "xdsettings.h"
#include "protoeditor.h"
#include "connection.h"
#include "sitesettings.h"
#include "phpsettings.h"
#include "debuggerstack.h"
#include "variable.h"
#include "debuggerbreakpoint.h"
#include "protoeditorsettings.h"
#include "session.h"

#include <kapplication.h>
#include <qhttp.h>
#include <kdebug.h>
#include <kurl.h>
#include <klocale.h>

#include <qregexp.h>
#include <qsocket.h>
#include <qdom.h>
#include <kurl.h>
#include <kmdcodec.h>

XDNet::XDNet(DebuggerXD* debugger, QObject *parent, const char *name)
    : QObject(parent, name), m_tempBreakpointId(0),
       m_site(0), m_debugger(debugger), m_con(0), m_socket(0)
{
  m_con = new Connection();
  connect(m_con, SIGNAL(sigAccepted(QSocket*)), this, SLOT(slotIncomingConnection(QSocket*)));
  connect(m_con, SIGNAL(sigClientClosed()), this, SLOT(slotXDClosed()));
  connect(m_con, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));
}

XDNet::~XDNet()
{
  delete m_con;
}

void XDNet::setSite(SiteSettings* site)
{
  m_site = site;
}

bool XDNet::startListener(int port)
{
  return m_con->listenOn(port);
}

void XDNet::stopListener()
{
  m_con->close();
}

void XDNet::startDebugging(const QString& filePath, const QString& uiargs,
    SiteSettings* site, bool local)
{
  int id = kapp->random();
  if(local) 
  {
    QStringList env;
    env << "XDEBUG_CONFIG"
        << ("remote_port=" 
            + QString::number(m_debugger->settings()->listenPort())
            + "\\ remote_host=localhost"
           )
        << "XDEBUG_SESSION_START"
        << QString::number(id);

    Protoeditor::self()->session()->startLocal(PHPSettings::lang, KURL::fromPathOrURL(filePath),uiargs, env);
  } 
  else
  {
    KURL url = site->mapRequestURLFor(filePath);

    QString query = QString("XDEBUG_SESSION_START=")+QString::number(id);
    
    if(!uiargs.isEmpty())
    {
      query += "&" + uiargs;
    }

    url.setQuery(query);
    Protoeditor::self()->session()->startRemote(url);
  }
}

void XDNet::requestContinue()
{
  QString run = "run -i ";
  run += QString::number(GeneralId);
  m_socket->writeBlock(run, run.length()+1);
}

void XDNet::requestStop()
{
  QString stop = "stop -i ";
  stop += QString::number(GeneralId);
  m_socket->writeBlock(stop, stop.length()+1);
  m_con->closeClient();
}

void XDNet::requestRunToCursor(const QString& filePath, int line)
{
  QString breakpoint_set = "breakpoint_set -t line -f ";

  if(m_site)
  {
    breakpoint_set += m_site->mapLocalToRemote(filePath);
  }
  else
  {
    breakpoint_set += filePath;
  }

  breakpoint_set += " -i ";
  breakpoint_set += QString::number(TempBpId);
  breakpoint_set += " -n ";
  breakpoint_set += QString::number(line);
  breakpoint_set += " -s ";
  breakpoint_set += "enabled";
  breakpoint_set += " -r 1";

  m_socket->writeBlock(breakpoint_set, breakpoint_set.length()+1);

  //request continue
  QString run = "run -i ";
  run += QString::number(RunToCursorId);
  m_socket->writeBlock(run, run.length()+1);  
}

void XDNet::requestStepInto()
{
  QString step_into = "step_into -i ";
  step_into += QString::number(GeneralId);
  m_socket->writeBlock(step_into, step_into.length()+1);
}

void XDNet::requestStepOver()
{
  QString step_over = "step_over -i ";
  step_over += QString::number(GeneralId);
  m_socket->writeBlock(step_over, step_over.length()+1);
}

void XDNet::requestStepOut()
{
  QString step_out = "step_out -i ";
  step_out += QString::number(GeneralId);
  m_socket->writeBlock(step_out, step_out.length()+1);
}

void XDNet::requestStack(int id)
{
  QString stack_get = "stack_get -i ";
  stack_get += QString::number(id);
  m_socket->writeBlock(stack_get, stack_get.length()+1);
}

void XDNet::requestVariables(int scope, int id)
{
  QString context_get = "context_get -d ";
  context_get += QString::number(scope);
  context_get += " -i ";
  context_get += QString::number(id);

  m_socket->writeBlock(context_get, context_get.length()+1);


  if(id == GlobalScopeId && m_debugger->settings()->sendSuperGlobals())
  {
    requestSuperGlobals(scope);
  }
}

void XDNet::requestSuperGlobals(int scope)
{
  //total global vars: 9
  m_superglobalsCount = 9;

  requestProperty("$GLOBALS", scope, SuperGlobalId);
  requestProperty("$_ENV", scope, SuperGlobalId);
  requestProperty("$_POST", scope, SuperGlobalId);
  requestProperty("$_GET", scope, SuperGlobalId);
  requestProperty("$_COOKIE", scope, SuperGlobalId);
  requestProperty("$_SERVER", scope, SuperGlobalId);
  requestProperty("$_FILES", scope, SuperGlobalId);
  requestProperty("$_REQUEST", scope, SuperGlobalId);
  requestProperty("$_SESSION", scope, SuperGlobalId);
}

void XDNet::requestWatch(const QString& expression, int ctx_id)
{
  requestProperty(expression, ctx_id, 1);
}

void XDNet::requestProperty(const QString& expression, int ctx_id, int id)
{
  QString property_get = "property_get -n ";
  property_get += expression;
  property_get += " -d ";
  property_get += QString::number(ctx_id);
  property_get += " -i ";
  property_get += QString::number(id);
  m_socket->writeBlock(property_get, property_get.length()+1);
}

void XDNet::requestBreakpoint(DebuggerBreakpoint* bp)
{
  //NOTE: xdebug2.0beta doesn't support conditional
  QString breakpoint_set = "breakpoint_set -t line -f ";

  if(m_site)
  {
    breakpoint_set += m_site->mapLocalToRemote(bp->url().path());
  }
  else
  {
    breakpoint_set += bp->url().path();
  }

  breakpoint_set += " -i ";
  breakpoint_set += QString::number(GeneralId);
  breakpoint_set += " -n ";
  breakpoint_set += QString::number(bp->line());
  breakpoint_set += " -s ";
  breakpoint_set += bp->status()==DebuggerBreakpoint::ENABLED?"enabled":"disabled";

//   breakpoint_set += " -h ";
//   breakpoint_set += QString::number(3);
//   breakpoint_set += " -- ";
//   breakpoint_set += KCodecs::base64Encode(QString(">=").utf8());

  m_socket->writeBlock(breakpoint_set, breakpoint_set.length()+1);
}

void XDNet::requestBreakpointUpdate(DebuggerBreakpoint* bp)
{
  QString breakpoint_update = "breakpoint_update -i ";
  breakpoint_update += QString::number(GeneralId);
  breakpoint_update += " -d ";
  breakpoint_update += QString::number(bp->id());
  breakpoint_update += " -s ";
  breakpoint_update += bp->status()==DebuggerBreakpoint::ENABLED?"enabled":"disabled";
//   breakpoint_update += " -h ";
//   breakpoint_update += QString::number(bp->condition().isEmpty()?0:1);
//   breakpoint_update += " -- ";
//   breakpoint_update += KCodecs::base64Encode(bp->condition().utf8());

  m_socket->writeBlock(breakpoint_update, breakpoint_update.length()+1);
}

void XDNet::requestBreakpointRemoval(int bpid)
{
  QString breakpoint_remove = "breakpoint_remove -d ";
  breakpoint_remove += QString::number(bpid);
  breakpoint_remove += " -i ";
  breakpoint_remove += QString::number(GeneralId);

  m_socket->writeBlock(breakpoint_remove, breakpoint_remove.length()+1);
}

void XDNet::requestBreakpointList()
{
  QString breakpoint_list = "breakpoint_list -i ";
  breakpoint_list += QString::number(GeneralId);
  m_socket->writeBlock(breakpoint_list, breakpoint_list.length()+1);
}

void XDNet::slotIncomingConnection(QSocket* socket)
{
  connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadBuffer()));

  m_socket = socket;  
  
  emit sigNewConnection();
}

// #include <iostream>

void XDNet::slotReadBuffer()
{
  long xmlSize;
  QString buffer;
  char ch;
  long read = 0;
  long totalread;
  QString str;

  do
  {
    buffer = "";

    while((ch = m_socket->getch()) != 0)
    {
      buffer += ch;
    }

    xmlSize = buffer.toLong();
    char data[xmlSize+1];

    totalread = 0;
    int aa = 99;
    while(totalread != xmlSize)
    { 
      read = m_socket->readBlock(&data[totalread], xmlSize-totalread);

      if(read == -1)
      {
        return;
      }

      //       if(read == 0) break;
      if(read == 0)
      {
        aa = m_socket->waitForMore (-1, 0L);
      }

      totalread += read;
    }


    data[xmlSize] = 0;
    
    str.setAscii(data,xmlSize);
//     std::cerr << "read: " << totalread << ", datalen: [" << xmlSize << "]>>>>\n" << data << "\n<<<\n" << std::endl;
    processXML(str);

  }
  while(m_socket && m_socket->bytesAvailable());
}

void XDNet::processXML(const QString& xml)
{
  QDomDocument* d = new QDomDocument();
  d->setContent(xml);

  QDomElement root = d->documentElement();
  QString name = root.tagName();

  if(name == "init")
  {
    processInit(root);
  }
  else if(name == "response")
  {
    processResponse(root);
  }
  else if(name == "stream")
  {
    processOutput(root);
  }

  delete d;
}

void XDNet::processOutput(QDomElement& output)
{
  //  <stream type="stdout" encoding="base64"><![CDATA[b2lcbnhhdQ==]]></stream>

  QString type = output.attribute("type");
  QString data;
  if(output.attribute("encoding") == "base64")
  {
    data = KCodecs::base64Decode(output.text().utf8());
  }
  else
  {
    data = output.text();
  }

  if(type == "stdout")
  {
    m_debugger->addOutput(data);
  }
//   else
//   {
//     error(QString("todo: ") + type);
//   }
}

void XDNet::processInit(QDomElement& init)
{

  QDomElement engine = init.namedItem("engine").toElement();
  QDomElement author = init.namedItem("author").toElement();
  QDomElement url = init.namedItem("url").toElement();
  QDomElement copyright = init.namedItem("copyright").toElement();

  kdDebug() << engine.text()
  << ": "
  << url.text()
  << " "
  << copyright.text()
  << endl;

  emit sigXDStarted();

  //setup stdout/sterr:

  QString stdout = "stdout -i 1 -c 1";//copy stdout to IDE
  m_socket->writeBlock(stdout, stdout.length()+1);

  QString stderr = "stderr -i 1 -c 1";//copy stderr to IDE
  m_socket->writeBlock(stderr, stderr.length()+1);  

  if(m_debugger->settings()->breakOnLoad())
  {
    requestStepInto();
  }
  else
  {
    requestContinue();
  }
}

void XDNet::processResponse(QDomElement& root)
{  
  QString cmd = root.attribute("command");

//   kdDebug() << "process response for cmd: " << cmd << ", status: " << root.attribute("status") << endl;

  if((cmd == "step_into") ||
      (cmd == "step_over") ||
      (cmd == "step_out") ||
      (cmd == "run"))
  {
    if(root.attributeNode("transaction_id").value().toInt() == RunToCursorId) 
    {
      requestBreakpointRemoval(m_tempBreakpointId);
      m_tempBreakpointId = 0;
    }

    if((root.attribute("status") == "break"))
    {      
      if(root.attribute("reason") == "error")
      {        
        processError(root.firstChild().toElement());
        requestStack(ErrorStackId); //see processErrorData() for why this is here
        requestContinue();
      }
      else
      {        
        requestStack(GeneralId);
        requestBreakpointList();
      }
    }
    else if(root.attribute("status") == "stopped")
    {
      //nothing...
    }
    else if(root.attribute("status") == "stopping")
    {
      if(root.attribute("reason") == "aborted") 
      {
        processError(root.firstChild().toElement());
        requestStack(ErrorStackId); //see processErrorData() for why this is here
        requestContinue();
      }
    }
    else
    {
      error(i18n("Unknown error\ncommand: ") + cmd +
            "\nstatus: " + root.attribute("status") +
            "\nreason: " + root.attribute("reason"));
    }
  }
  else if(cmd == "stack_get")
  {
    QDomNodeList list = root.elementsByTagName("stack");
    QDomElement st;
    DebuggerStack* stack = new DebuggerStack();
    for(uint i = 0; i < list.count(); i++)
    {
      st = list.item(i).toElement();
      int level = st.attributeNode("level").value().toInt();
      int line = st.attributeNode("lineno").value().toInt();

      QString file = st.attributeNode("filename").value();
      QString where = st.attributeNode("where").value();

      if((where == "{main}") || (where == "include"))
      {
        where = file + i18n("::main()");
      }

      //to local filepath
      KURL localFile;
//       QString localFile;

      if(m_site)
      {
        localFile = m_site->mapRemoteToLocal(file);
      }
      else
      {
        localFile = KURL::fromPathOrURL(file);
      }
      stack->insert(level, localFile, line, where);
    }

    //request global/local vars
    //requestVariables(stack->bottomExecutionPoint()->id(), XDNet::GLOBAL_SCOPE);
    //requestVariables(stack->topExecutionPoint()->id(), XDNet::LOCAL_SCOPE);


    //update stack
    if(root.attributeNode("transaction_id").value().toInt() == ErrorStackId)
    {
      m_error.filePath = stack->topExecutionPoint()->url().path();
      m_error.line     = stack->topExecutionPoint()->line();
      dispatchErrorData();
    }
    else
    {    
      m_debugger->updateStack(stack);
      emit sigStepDone();
    }    
  }
  else if(cmd == "context_get")
  {
    QDomNodeList list = root.childNodes();
    XDVariableParser p;

    VariableList_t* array = p.parse(list);

    if(root.attributeNode("transaction_id").value().toInt() == LocalScopeId)
    {
      m_debugger->updateVariables(array, false);
    }
    else if(root.attributeNode("transaction_id").value().toInt() == GlobalScopeId)
    {
      if(root.attributeNode("transaction_id").value().toInt() ==
          GlobalScopeId && m_debugger->settings()->sendSuperGlobals())
      {
        m_globalVars = array;
      }
      else
      {
        m_debugger->updateVariables(array, true);
      }
    } 
    else 
    {
      error(i18n("Xdebug client: Internal error"));
    }
  }
  else if(cmd == "property_get")
  {
    QDomNode nd = root.firstChild();
    XDVariableParser p;

    Variable* var = p.parse(nd);
    if(root.attributeNode("transaction_id").value().toInt() == SuperGlobalId)
    {
      m_globalVars->append(var);
      m_superglobalsCount--;

      if(m_superglobalsCount == 0)
      {
        m_debugger->updateVariables(m_globalVars, true);
      }
    }
    else
    {
      m_debugger->updateWatch(var);
    }
  }
  else if((cmd == "stop") ||
          (cmd == "breakpoint_remove") ||
          (cmd == "stdout") ||
          (cmd == "stderr"))
  {
    //nothing..
  }
  else if((cmd == "breakpoint_set") || (cmd == "breakpoint_update"))
  {
    if(root.attributeNode("transaction_id").value().toInt() == TempBpId)
    {
      m_tempBreakpointId = root.attributeNode("id").value().toLong();
    }
    else
    {
      //do not request for breakpoint list if the one we just setted is a 
      //temporary one
      requestBreakpointList();
    }
  }
  else if(cmd == "breakpoint_list")
  {
    /*
    <response command="breakpoint_list" transaction_id="1">
      <breakpoint type="line" filename="file:///usr/local/apache/htdocs/texto.php"
        lineno="9" state="enabled" id="157470001">
      </breakpoint>
      <breakpoint type="line" filename="file:///usr/local/apache/htdocs/texto.php" lineno="10" state="enabled" id="157470002">
      </breakpoint>
    </response>

    */
    QDomNodeList list = root.elementsByTagName("breakpoint");
    QDomElement e;
    for(uint i = 0; i < list.count(); i++)
    {
      e = list.item(i).toElement();

      int id = e.attributeNode("id").value().toInt();
//       if(id == m_tempBreakpointId)  
//       {
//         //do not dispatch this BP. It's a temp one (run to cursor)
//         continue;
//       }

      QString filePath = KURL::fromPathOrURL(e.attributeNode("filename").value()).path();

      if(m_site)
      {
        filePath = m_site->mapRemoteToLocal(filePath);
      }

      int line = e.attributeNode("lineno").value().toInt();
      QString state = e.attributeNode("state").value();
      int hitCount = e.attributeNode("hit_count").value().toInt();
      int skip = e.attributeNode("hit_value").value().toInt();
      QString condition = e.attributeNode("condition").value();

      m_debugger->updateBreakpoint(id, filePath, line, state, hitCount, skip, condition);
    }
  }
  else
  {
    error(i18n("Unknow network packet: ") + cmd);
  }
}

void XDNet::processError(const QDomElement& error)
{
  /*
    <response command="step_into" transaction_id="1" status="break" reason="error">
    <error code="2" exception="Warning">
    <![CDATA[array_push() [<a href='function.array-push'>function.array-push</a>]: First argument should be an array]]>
    </error></response>
  */

  m_error.code = error.attribute("code");
  m_error.exception = error.attribute("exception");

  if(error.attribute("encoding") == "base64")
  {
    m_error.data = KCodecs::base64Decode(error.text().utf8());
  }
  else
  {
    m_error.data  = error.text();
  }

  /* error will be sent to m_debugger after stack is updated. See processErrorData()
     reason: so the error can be sync with the currentExecutionPoint (wich is the one that tells where
     the error ocurred
  */
}

void XDNet::dispatchErrorData()
{
  QRegExp rx;

  rx.setPattern("\\[[^\\]]*\\]");
  m_error.data.remove(rx);

  m_debugger->debugError(m_error.code.toInt(), m_error.filePath, m_error.line, m_error.data);
}

void XDNet::slotXDClosed()
{
  emit sigXDClosed();
}

void XDNet::slotError(const QString& msg)
{
  error(msg);
}

void XDNet::error(const QString& msg)
{
  emit sigError(msg);
  m_con->closeClient();
}

#include "xdnet.moc"
