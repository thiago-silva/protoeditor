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

#include "xdnet.h"
#include "debuggerxd.h"
#include "xdvariableparser.h"
#include "xdsettings.h"

#include "connection.h"
#include "sitesettings.h"
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

#include <qsocket.h>
#include <qdom.h>
#include <kurl.h>
#include <kmdcodec.h>

XDNet::XDNet(DebuggerXD* debugger, QObject *parent, const char *name)
    : QObject(parent, name), m_debugger(debugger), m_con(0), /*m_http(0)*/ m_socket(0)
{
  m_error.exists = false;

  connect(Session::self(), SIGNAL(sigError(const QString&)),
      this, SLOT(slotError(const QString&)));

  m_con = new Connection();
  connect(m_con, SIGNAL(sigAccepted(QSocket*)), this, SLOT(slotIncomingConnection(QSocket*)));
  connect(m_con, SIGNAL(sigClientClosed()), this, SLOT(slotXDClosed()));
  connect(m_con, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));
}

XDNet::~XDNet()
{
  delete m_con;
}

bool XDNet::startListener(int port)
{
  return m_con->listenOn(port);
}

void XDNet::stopListener()
{
  m_con->close();
}

void XDNet::startDebugging(const QString& filePath, SiteSettings* site, bool local)
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

    Session::self()->start(filePath, env, true);
  } 
  else
  {
    QString uri = filePath;
    uri = uri.remove(0, site->localBaseDir().length());

    KURL url = site->effectiveURL();
    url.setPath(url.path() + uri);
    url.setQuery(QString("XDEBUG_SESSION_START=")+QString::number(id));
  
    Session::self()->start(url);    
  }
}

void XDNet::requestContinue()
{
  QString run = "run -i 1";
  m_socket->writeBlock(run, run.length()+1);
}

void XDNet::requestStop()
{
  QString stop = "stop -i ";
  stop += QString::number(GeneralId);
  m_socket->writeBlock(stop, stop.length()+1);
  m_con->closeClient();
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

void XDNet::requestStack()
{
  QString stack_get = "stack_get -i ";
  stack_get += QString::number(GeneralId);
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
  QString breakpoint_set = "breakpoint_set -t line -f ";
  breakpoint_set += bp->filePath().section('/', -1);
  breakpoint_set += " -i ";
  breakpoint_set += QString::number(GeneralId);
  breakpoint_set += " -n ";
  breakpoint_set += QString::number(bp->line());
  breakpoint_set += " -h ";
  breakpoint_set += QString::number(bp->skipHits());
  breakpoint_set += " -s ";
  breakpoint_set += bp->status()==DebuggerBreakpoint::ENABLED?"enabled":"disabled";
  breakpoint_set += " -- ";
  breakpoint_set += KCodecs::base64Encode(bp->condition().utf8());;

  m_socket->writeBlock(breakpoint_set, breakpoint_set.length()+1);
}

void XDNet::requestBreakpointUpdate(DebuggerBreakpoint* bp)
{
  QString breakpoint_update = "breakpoint_update -i 1 -d ";
  breakpoint_update += QString::number(bp->id());
  breakpoint_update += " -i ";
  breakpoint_update += QString::number(GeneralId);
  breakpoint_update += " -h ";
  breakpoint_update += QString::number(bp->skipHits());
  breakpoint_update += " -s ";
  breakpoint_update += bp->status()==DebuggerBreakpoint::ENABLED?"enabled":"disabled";
  breakpoint_update += " -- ";
  breakpoint_update += KCodecs::base64Encode(bp->condition().utf8());;

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

  emit sigXDStarted();
}

#include <iostream>

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
    int aa;
    while(totalread != xmlSize)
    {
      aa = m_socket->bytesAvailable();
      read = m_socket->readBlock(&data[totalread], xmlSize-totalread);

      if(read == -1)
      {
        return;
      }

      //       if(read == 0) break;
      if(read == 0)
      {
        m_socket->waitForMore (-1, 0L);
      }

      totalread += read;
    }


    data[xmlSize] = 0;
    //     if(m_superglobalsCount == 9) {
    //       std::cerr << "read: " << totalread << ", datalen: [" << xmlSize << "]>>>>\n" << data << "\n<<<\n" << std::endl;
    //     }

    str.setAscii(data,xmlSize);
    processXML(str);

  }
  while(m_socket->bytesAvailable());
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
  else
  {
    error(QString("todo: ") + type);
  }
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

  if((cmd == "step_into") ||
      (cmd == "step_over") ||
      (cmd == "step_out") ||
      (cmd == "run"))
  {
    if((root.attribute("status") == "break"))
    {
      if(root.attribute("reason") == "error")
      {
        processError(root.firstChild().toElement());
      }

      requestStack();
      requestBreakpointList();
    }
    else if(root.attribute("status") == "stopped")
    {
      //nothing...
    }
    else
    {
      error(QString("Unknown error\ncommand: ") + cmd +
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

      KURL file = st.attributeNode("filename").value();
      QString where = st.attributeNode("where").value();

      if((where == "{main}") || (where == "include"))
      {
        where = file.path() + "::main()";
      }

      //to local filepath
      QString localFile;

      SiteSettings* site = ProtoeditorSettings::self()->currentSiteSettings();
      if(site) 
      {
        localFile = site->localBaseDir()
                            + file.path().remove(0, site->remoteBaseDir().length());
      }
      else
      {
        localFile = file.path();
      }
      stack->insert(level, localFile, line, where);
    }

    //request global/local vars
    //requestVariables(stack->bottomExecutionPoint()->id(), XDNet::GLOBAL_SCOPE);
    //requestVariables(stack->topExecutionPoint()->id(), XDNet::LOCAL_SCOPE);


    //update stack
    m_debugger->updateStack(stack);

    emit sigStepDone();

    processPendingData();
  }
  else if(cmd == "context_get")
  {
    QDomNodeList list = root.childNodes();
    XDVariableParser p;

    VariablesList_t* array = p.parse(list);

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
    requestBreakpointList();
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
      QString filePath = KURL(e.attributeNode("filename").value()).path();
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
    error(QString("Unknow network packet: ") + cmd);
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

  m_error.exists = true;

  /* error will be sent to m_debugger after stack is updated. See processPendingData()
     reason: so the error can be sync with the currentExecutionPoint (wich is the one that tells where
     the error ocurred
  */
}

void XDNet::processPendingData()
{
  if(m_error.exists)
  {
    m_debugger->debugError(m_error.code, m_error.exception, m_error.data);
    m_error.exists = false;
  }
}

void XDNet::slotXDClosed()
{
  //stoping the debugger (not the session!!)

  emit sigXDClosed();

  //   m_receiver->clear();
  //   m_requestor->clear();

  //   m_dbgStack->clear();
  //   m_dbgFileInfo->clear();
  //   m_varScopeRequestList.clear();

  //   m_isProfiling = false;
  //   m_setupProfile = false;
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
