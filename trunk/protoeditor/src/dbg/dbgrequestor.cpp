/***************************************************************************
 *   Copyright (C) 2004 by Thiago Silva                                    *
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


#include "dbgrequestor.h"

#include "debuggerbreakpoint.h"
#include "dbg_defs.h"
#include "dbgnetdata.h"
#include "dbgrequestpack.h"
#include "dbgrequestpackbuilder.h"
#include "browsersettings.h"

#include <qsocket.h>
#include <qhttp.h>
#include <klocale.h>
#include <kdebug.h>

#include <kurl.h>
#include <kapplication.h>
#include <kprocess.h>
#include <dcopclient.h>

DBGRequestor::DBGRequestor()
    : QObject(), m_socket(NULL)/*, m_http(0)*/, m_headerFlags(0) /*, m_terminating(false)*/
{
  m_browser = new Browser();

  connect(m_browser, SIGNAL(sigError(const QString&)),
          this, SIGNAL(sigError(const QString&)));
}

DBGRequestor::~DBGRequestor()
{
  delete m_browser;
}

void DBGRequestor::requestContinue()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_CONTINUE);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestStop()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STOP);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestStepInto()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPINTO);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestStepOver()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPOVER);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestStepOut()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildCommand(DBGA_STEPOUT);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestWatch(const QString& expression,int scope_id)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildWatch(expression, scope_id);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestBreakpoint(int bpno, int modno, const QString& remoteFilePath, int line, const QString& condition, int status, int skiphits)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildBreakpoint(
                                  bpno, modno, remoteFilePath, line, condition, status, skiphits);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestBreakpointRemoval(int bpid)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildDeletedBreakpoint(bpid);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestVariables(int scope_id)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildVars(scope_id);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestSrcTree()
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildSrcTree();
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::requestOptions(int op)
{
  if(!m_socket) return;

  DBGRequestPack* requestPack = DBGRequestPackBuilder::buildOptions(op);
  requestPack->header()->setFlags(m_headerFlags);
  requestPack->send(m_socket);
  delete requestPack;
}

void DBGRequestor::makeHttpRequest(const QString& host, const QString& path, int listenPort, int sessionId)
{
  QString reqUrl = QString("http://") + host + path + "?DBGSESSID="
                   + QString::number(sessionId)
                   + "@clienthost:"
                   + QString::number(listenPort);

  m_browser->request(reqUrl);
}

void DBGRequestor::addHeaderFlags(dbgint flags)
{
  m_headerFlags |= flags;
}

void DBGRequestor::setSocket(QSocket* socket)
{
  m_socket = socket;
}

void DBGRequestor::clear()
{
  m_socket = NULL;
}

/********************************************************************************/

Browser::Browser()
    : m_http(0), m_browserProcess(0), m_dcopClient(0), m_processRunning(false)
{}

Browser::~Browser()
{
  if(m_browserProcess) m_browserProcess->kill();

  delete m_browserProcess;
  delete m_http;
}

void Browser::request(const QString& url)
{
  if(BrowserSettings::useExternalBrowser()) {
    doBrowserRequest(url);
  } else {
    doHTTPRequest(KURL(url));
  }
}

void Browser::doBrowserRequest(const QString& url)
{
  initBrowserCommunication();

  if(!m_processRunning) {
    openURLOnBrowser(url);
    return;
  }

  QString cmd = BrowserSettings::browserCmd();

  QByteArray data;
  QDataStream arg(data, IO_WriteOnly);
  arg << url;

  kdDebug() << "asking for browser ("
  << m_browserProcess->pid()
  << ") to open URL: "
  << url
  << endl;

  if(!m_dcopClient->send(
       (cmd + "-" + QString::number(m_browserProcess->pid())).ascii()
       , "konqueror-mainwindow#1", "openURL(QString)"
       , data)) {
    emit sigError("Error opening browser");
  }
}

void Browser::initBrowserCommunication()
{
  if(!m_dcopClient) {
    m_dcopClient = KApplication::dcopClient();

    if(!m_dcopClient->attach()) {
      emit sigError("Could not init browser communication");
    }
  }

  if(!m_processRunning) {

    if(m_browserProcess) {
      delete m_browserProcess;
    }

    m_browserProcess = new KProcess;
    connect(m_browserProcess, SIGNAL(processExited(KProcess*)),
      this, SLOT(slotProcessExited(KProcess*)));
  }
}

void Browser::openURLOnBrowser(const QString& url)
{
  QString cmd = BrowserSettings::browserCmd();

  *m_browserProcess << cmd;
  *m_browserProcess << url;

  kdDebug() << "processing browser request: " << cmd << " " << url << endl;

  if(!m_browserProcess->start()) {
    emit sigError("Error opening browser");
  } else {
    kdDebug() << "New browser PID: " << m_browserProcess->pid() << endl;
    m_processRunning = true;
  }
}

void Browser::doHTTPRequest(const KURL& url)
{
  initHTTPCommunication();

  kdDebug() << "HTTP request \"" << url.path() + url.query() <<  "\" from " << url.host() << endl;

  m_http->setHost(url.host());
  m_http->get(url.path() + url.query());
}

void Browser::initHTTPCommunication()
{
  if(!m_http) {
    m_http = new QHttp;
    connect(m_http, SIGNAL(done(bool)), this, SLOT(slotHttpDone(bool)));
  }
}

void Browser::slotHttpDone(bool error)
{
  if(error) {
    emit sigError(i18n("HTTP Conection error: " + m_http->errorString()));
  }
}

void Browser::slotProcessExited(KProcess*)
{
  //proc->kill();
  m_processRunning = false;
}

#include "dbgrequestor.moc"
