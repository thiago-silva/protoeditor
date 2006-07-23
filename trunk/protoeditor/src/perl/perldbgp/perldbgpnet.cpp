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

#include "perldbgpnet.h"
#include "debuggerperldbgp.h"
#include "perldbgpvariableparser.h"
#include "perldbgpsettings.h"
#include "protoeditor.h"
#include "connection.h"
#include "sitesettings.h"
#include "debuggerstack.h"
#include "variable.h"
#include "debuggerbreakpoint.h"
#include "protoeditorsettings.h"
#include "session.h"
#include "languagesettings.h"

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

PerlDBGPNet::PerlDBGPNet(DebuggerPerlDBGP* debugger, QObject *parent, const char *name)
    : QObject(parent, name), m_tempBreakpointId(0),
       m_site(0), m_debugger(debugger), m_con(0), m_socket(0)
{
  m_con = new Connection();
  connect(m_con, SIGNAL(sigAccepted(QSocket*)), this, SLOT(slotIncomingConnection(QSocket*)));
  connect(m_con, SIGNAL(sigClientClosed()), this, SLOT(slotClosed()));
  connect(m_con, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));
}

PerlDBGPNet::~PerlDBGPNet()
{
  delete m_con;
}

void PerlDBGPNet::setSite(SiteSettings* site)
{
  m_site = site;
}

bool PerlDBGPNet::startListener(int port)
{
  return m_con->listenOn(port);
}

void PerlDBGPNet::stopListener()
{
  m_con->close();
}

void PerlDBGPNet::startDebugging(const QString& filePath, const QString& uiargs,
    SiteSettings* site, bool local)
{
  int id = kapp->random();
  if(local) 
  {
    QStringList env;
    env << "PERLDB_OPTS" << ("RemotePort=localhost:" + QString::number(m_debugger->settings()->listenPort()))
        << "PERL5DB" << ("BEGIN { require q(" + m_debugger->settings()->perlDBGPLibPath() + "perl5db.pl) }")
        << "PERL5LIB" << m_debugger->settings()->perlDBGPLibPath()
        << "DBGP_KEY" << QString::number(id);
    
    // /usr/bin/perl -d -w -I/home/jester/apps/komodo/lib/support/dbgp/perllib /home/jester/teste.pl
    QString cmd = m_debugger->langSettings()->interpreterCommand();
    cmd += " -d -w -I";
    cmd += m_debugger->settings()->perlDBGPLibPath();    
    Protoeditor::self()->session()->startLocal(cmd, KURL::fromPathOrURL(filePath),uiargs, env);
  } 
  else
  {
    KURL url = site->mapRequestURLFor(filePath);
    Protoeditor::self()->session()->startRemote(url);
  }
}

void PerlDBGPNet::requestContinue()
{
  QString run = "run -i ";
  run += QString::number(GeneralId);
  m_socket->writeBlock(run, run.length()+1);
}

void PerlDBGPNet::requestStop()
{
  QString stop = "stop -i ";
  stop += QString::number(GeneralId);
  m_socket->writeBlock(stop, stop.length()+1);
//   m_con->closeClient();
}

void PerlDBGPNet::requestRunToCursor(const QString& filePath, int line)
{
  QString breakpoint_set = "breakpoint_set -t line -f ";

  if(m_site)
  {
    breakpoint_set += m_site->mapLocalToRemote(filePath);
  }
  else
  {
    breakpoint_set += "file:///" + filePath;
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

void PerlDBGPNet::requestStepInto()
{
  QString step_into = "step_into -i ";
  step_into += QString::number(GeneralId);
  m_socket->writeBlock(step_into, step_into.length()+1);
}

void PerlDBGPNet::requestStepOver()
{
  QString step_over = "step_over -i ";
  step_over += QString::number(GeneralId);
  m_socket->writeBlock(step_over, step_over.length()+1);
}

void PerlDBGPNet::requestStepOut()
{
  QString step_out = "step_out -i ";
  step_out += QString::number(GeneralId);
  m_socket->writeBlock(step_out, step_out.length()+1);
}

void PerlDBGPNet::requestStack(int id)
{
  QString stack_get = "stack_get -i ";
  stack_get += QString::number(id);
  m_socket->writeBlock(stack_get, stack_get.length()+1);
}

void PerlDBGPNet::requestVariables(int scope, int id)
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

void PerlDBGPNet::requestSuperGlobals(int scope)
{
  //total global vars: 6
  m_superglobalsCount = 6;
  requestProperty("$0", scope, SuperGlobalId);
  requestProperty("%ENV", scope, SuperGlobalId);
  requestProperty("%INC", scope, SuperGlobalId);
  requestProperty("@INC", scope, SuperGlobalId);
  requestProperty("%SIG", scope, SuperGlobalId);
  requestProperty("@_", scope, SuperGlobalId);
}

void PerlDBGPNet::requestModifyVar(Variable* var, int scope)
{
  QString value = dynamic_cast<VariableScalarValue*>(var->value())->toString();
 
  QString property_set = "property_set -n ";
  property_set += var->compositeName();
  property_set += " -d ";
  property_set += QString::number(scope);
  property_set += " -i " ;
  property_set += QString::number(GeneralId);
  property_set += " -- ";
  property_set += KCodecs::base64Encode(value.utf8());
  m_socket->writeBlock(property_set, property_set.length()+1);
}

void PerlDBGPNet::requestWatch(const QString& expression, int ctx_id)
{
  requestProperty(expression, ctx_id, 1);
}

void PerlDBGPNet::requestChildren(int scopeId, Variable* var)
{
  m_updateVars.append(var);
  switch(scopeId)
  {
    case DebuggerStack::GlobalScopeID:
      requestProperty(var->compositeName(), PerlDBGPNet::GlobalScopeId, GlobalChildId);
      break;
    case DebuggerStack::LocalScopeID:
      requestProperty(var->compositeName(), PerlDBGPNet::LocalScopeId, LocalChildId);
      break;
    default:
      //error! no such scope
      break;
  }
}

void PerlDBGPNet::requestProperty(const QString& expression, int ctx_id, int id)
{
  QString property_get = "property_get -n ";
  property_get += expression;
  property_get += " -d ";
  property_get += QString::number(ctx_id);
  property_get += " -i ";
  property_get += QString::number(id);
  m_socket->writeBlock(property_get, property_get.length()+1);
}

void PerlDBGPNet::requestBreakpoint(DebuggerBreakpoint* bp)
{
  //breakpoint_set -t line -f file:///home/jester/teste.php -n 3 -s enabled -r 0 -h 0 -i 18.
  QString breakpoint_set = "breakpoint_set -t line -f ";

  if(m_site)
  {
    breakpoint_set += m_site->mapLocalToRemote(bp->url().path());
  }
  else
  {
    breakpoint_set += bp->url().prettyURL();
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

void PerlDBGPNet::requestBreakpointUpdate(DebuggerBreakpoint* bp)
{
  QString breakpoint_update = "breakpoint_update -i ";
  breakpoint_update += QString::number(GeneralId);
  breakpoint_update += " -d ";
  breakpoint_update += QString::number(bp->id());
  breakpoint_update += " -s ";
  breakpoint_update += bp->status()==DebuggerBreakpoint::ENABLED?"enabled":"disable";
//   breakpoint_update += " -h ";
//   breakpoint_update += QString::number(bp->condition().isEmpty()?0:1);
//   breakpoint_update += " -- ";
//   breakpoint_update += KCodecs::base64Encode(bp->condition().utf8());

  m_socket->writeBlock(breakpoint_update, breakpoint_update.length()+1);
}

void PerlDBGPNet::requestBreakpointRemoval(int bpid)
{
  QString breakpoint_remove = "breakpoint_remove -d ";
  breakpoint_remove += QString::number(bpid);
  breakpoint_remove += " -i ";
  breakpoint_remove += QString::number(GeneralId);

  m_socket->writeBlock(breakpoint_remove, breakpoint_remove.length()+1);
}

void PerlDBGPNet::requestBreakpointList()
{
  QString breakpoint_list = "breakpoint_list -i ";
  breakpoint_list += QString::number(GeneralId);
  m_socket->writeBlock(breakpoint_list, breakpoint_list.length()+1);
}

void PerlDBGPNet::slotIncomingConnection(QSocket* socket)
{
  connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadBuffer()));

  m_socket = socket;  
  
  emit sigNewConnection();
}

// #include <iostream>

void PerlDBGPNet::slotReadBuffer()
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

void PerlDBGPNet::processXML(const QString& xml)
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

void PerlDBGPNet::processOutput(QDomElement& output)
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

  if((type == "stdout") || type == "stderr")
  {
    m_debugger->addOutput(data);
  }
}

void PerlDBGPNet::processInit(QDomElement& init)
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

  QString cmd = "stdout -i 1 -c 1";//copy stdout to IDE
  m_socket->writeBlock(cmd, cmd.length()+1);

  cmd = "stderr -i 1 -c 1";//copy stderr to IDE
  m_socket->writeBlock(cmd, cmd.length()+1);  

  cmd = "feature_set -n max_children -v 300 -i 1";//copy stderr to IDE
  m_socket->writeBlock(cmd, cmd.length()+1);    

  emit sigStarted();
 
  if(m_debugger->settings()->breakOnLoad())
  {
    requestStepInto();
  }
  else
  {
    requestContinue();
  }
}

void PerlDBGPNet::processResponse(QDomElement& root)
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
      else if(root.attribute("reason") == "ok")
      {
        requestStop();
      }
    }
    else
    {
//       error(i18n("Unknown error\ncommand: ") + cmd +
//             "\nstatus: " + root.attribute("status") +
//             "\nreason: " + root.attribute("reason"));
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

      if(where == "main")
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
    PerlDBGPVariableParser p;

    VariableList_t* array = p.parse(list);
    int trans = root.attributeNode("transaction_id").value().toInt();
    if(trans == LocalScopeId)
    {
      m_debugger->updateVariables(array, false);
    }
    else if(trans == GlobalScopeId)
    {
      if((trans == GlobalScopeId) && (m_debugger->settings()->sendSuperGlobals()))
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
      error(i18n("Debug client: Internal error"));
    }
  }
  else if(cmd == "property_get")
  {
    QDomNode nd = root.firstChild();
    PerlDBGPVariableParser p;    
    Variable* var;
    VariableList_t* list;
    Variable* updateVar;

    int trans = root.attributeNode("transaction_id").value().toInt();
    switch(trans)
    {
      case SuperGlobalId:
        var = p.parse(nd);
        m_globalVars->append(var);
        m_superglobalsCount--;
  
        if(m_superglobalsCount == 0)
        {
          m_debugger->updateVariables(m_globalVars, true);
        }
        break;

      case GlobalChildId:        
      case LocalChildId:
        updateVar = m_updateVars.first();
        m_updateVars.remove();
        list = p.parseList(nd.childNodes(), updateVar);
        dynamic_cast<PerlListValue*>(updateVar->value())->setList(list);
        if(trans == LocalChildId) {
          m_debugger->updateVariable(updateVar, DebuggerStack::LocalScopeID);
        } else {
          m_debugger->updateVariable(updateVar, DebuggerStack::GlobalScopeID);
        }
        break;
      default:
        var = p.parse(nd);
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
//     error(i18n("Unknow network packet: ") + cmd);
  }
}

void PerlDBGPNet::processError(const QDomElement& error)
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

void PerlDBGPNet::dispatchErrorData()
{
  QRegExp rx;

  rx.setPattern("\\[[^\\]]*\\]");
  m_error.data.remove(rx);

  m_debugger->debugError(m_error.code.toInt(), m_error.filePath, m_error.line, m_error.data);
}

void PerlDBGPNet::slotClosed()
{
  m_socket = 0;
  emit sigClosed();  
  m_updateVars.clear();
}

void PerlDBGPNet::slotError(const QString& msg)
{
  error(msg);
}

void PerlDBGPNet::error(const QString& msg)
{
  emit sigError(msg);
  m_con->closeClient();
}

#include "perldbgpnet.moc"
