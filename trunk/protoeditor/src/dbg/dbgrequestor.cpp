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
#include "settings.h"

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

/***************************** BROWSER ***************************************************/

Browser::Browser()
    : m_http(0), m_browserRequestor(0)
{}

Browser::~Browser()
{
  delete m_browserRequestor;
  delete m_http;
}

void Browser::request(const QString& url)
{
  if(Settings::useExternalBrowser()) {
    doBrowserRequest(url);
  } else {
    doHTTPRequest(KURL(url));
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

void Browser::doBrowserRequest(const QString& url)
{
  m_browserRequestor = BrowserRequestor::retrieveBrowser(Settings::browser(), this);
  //TODO: assert-me
  m_browserRequestor->doRequest(url);
}

/***************************** BROWSER REQUESTOR *****************************************/

BrowserRequestor* BrowserRequestor::m_browserRequestor = 0;

BrowserRequestor::BrowserRequestor()
 : m_browserProcess(0), m_processRunning(false)

{}

BrowserRequestor::~BrowserRequestor()
{
  delete m_browserProcess;
}

BrowserRequestor* BrowserRequestor::retrieveBrowser(int browser, Browser* br)
{
  switch(browser) {
    case Settings::EnumBrowser::Konqueror:
      if(m_browserRequestor && (m_browserRequestor->id() == Settings::EnumBrowser::Konqueror)) {
        return m_browserRequestor;
      }
      delete m_browserRequestor;
      m_browserRequestor = new KonquerorRequestor();
      connect(m_browserRequestor, SIGNAL(sigError(const QString&)), br,
        SIGNAL(sigError(const QString&)));
      break;
    case Settings::EnumBrowser::Mozilla:
      if(m_browserRequestor && (m_browserRequestor->id() == Settings::EnumBrowser::Mozilla)) {
        return m_browserRequestor;
      }
      delete m_browserRequestor;
      m_browserRequestor = new MozillaRequestor();
      connect(m_browserRequestor, SIGNAL(sigError(const QString&)), br,
        SIGNAL(sigError(const QString&)));
      break;
    case Settings::EnumBrowser::Firefox:
      if(m_browserRequestor && (m_browserRequestor->id() == Settings::EnumBrowser::Firefox)) {
        return m_browserRequestor;
      }
      delete m_browserRequestor;
      m_browserRequestor = new FirefoxRequestor();
      connect(m_browserRequestor, SIGNAL(sigError(const QString&)), br,
        SIGNAL(sigError(const QString&)));
      break;
    case Settings::EnumBrowser::Opera:
      if(m_browserRequestor && (m_browserRequestor->id() == Settings::EnumBrowser::Opera)) {
        return m_browserRequestor;
      }
      delete m_browserRequestor;
      m_browserRequestor = new OperaRequestor();
      connect(m_browserRequestor, SIGNAL(sigError(const QString&)), br,
        SIGNAL(sigError(const QString&)));
      break;
  }
  return m_browserRequestor;
}

void BrowserRequestor::init()
{
  if(!m_processRunning) {

    delete m_browserProcess;

    m_browserProcess = new KProcess;
    connect(m_browserProcess, SIGNAL(processExited(KProcess*)),
            this, SLOT(slotProcessExited(KProcess*)));
  }
}

void BrowserRequestor::slotProcessExited(KProcess*)
{
  //proc->kill();
  m_processRunning = false;
}


/***************************** KONQUEROR *****************************************/

KonquerorRequestor::KonquerorRequestor()
  : BrowserRequestor(), m_dcopClient(0)
{
}
KonquerorRequestor::~KonquerorRequestor()
{
}

void KonquerorRequestor::doRequest(const QString& url)
{
  init();

  if(!m_processRunning) {
    openNewKonqueror(url);
    return;
  }

  QByteArray data;
  QDataStream arg(data, IO_WriteOnly);
  arg << url;

  kdDebug() << "asking for konqueror ("
  << m_browserProcess->pid()
  << ") to open URL: "
  << url
  << endl;

  QString cmd = "konqueror";

  if(!m_dcopClient->send(
       (cmd + "-" + QString::number(m_browserProcess->pid())).ascii()
       , "konqueror-mainwindow#1", "openURL(QString)"
       , data)) {
    emit sigError("Error opening browser");
  }
}

void KonquerorRequestor::openNewKonqueror(const QString& url)
{
  QString cmd = "konqueror";
  *m_browserProcess << cmd;
  *m_browserProcess << url;

  kdDebug() << "creating new Konqueror window for: " << url << endl;

  if(!m_browserProcess->start()) {
    emit sigError("Error opening Konqueror");
  } else {
    kdDebug() << "New konqueror PID: " << m_browserProcess->pid() << endl;
    m_processRunning = true;
  }
}

int KonquerorRequestor::id()
{
  return Settings::EnumBrowser::Konqueror;
}

void KonquerorRequestor::init()
{
  BrowserRequestor::init();

  if(!m_dcopClient) {
    m_dcopClient = KApplication::dcopClient();

    if(!m_dcopClient->attach()) {
      emit sigError("Could not init browser communication");
    }
  }
}


/******************* MOZILLA *********************************/
MozillaRequestor::MozillaRequestor()
  : BrowserRequestor()
{
}
MozillaRequestor::~MozillaRequestor()
{
}

void MozillaRequestor::doRequest(const QString& url)
{
  init();

  QString arg = "openURL(" + url + ")";

  if(m_processRunning) {
    m_browserProcess->detach();
  }

  m_browserProcess->clearArguments();
  *m_browserProcess  << "mozilla";
  *m_browserProcess  << "-remote";
  *m_browserProcess << arg;


  if(!m_browserProcess->start(KProcess::Block)) {
    emit sigError("Error executing remote Mozilla");
  } else {
    if(m_browserProcess->exitStatus() != 0) {
      //no mozilla instance? Create new one
      m_browserProcess->clearArguments();
      *m_browserProcess  << "mozilla";
      *m_browserProcess  << url;
      if(!m_browserProcess->start()) {
        emit sigError("Error executing Mozilla");
      } else {
        m_processRunning = true;
      }
    }
  }
}

int MozillaRequestor::id()
{
  return Settings::EnumBrowser::Mozilla;
}


/************************ FIREFOX ***********************************************/
FirefoxRequestor::FirefoxRequestor()
  : BrowserRequestor()
{
}
FirefoxRequestor::~FirefoxRequestor()
{
}

void FirefoxRequestor::doRequest(const QString& url)
{
  init();

  QString arg = "openURL(" + url + ")";

  if(m_processRunning) {
    m_browserProcess->detach();
  }

  m_browserProcess->clearArguments();
  *m_browserProcess  << "firefox";
  *m_browserProcess  << "-remote";
  *m_browserProcess << arg;


  if(!m_browserProcess->start(KProcess::Block)) {
    emit sigError("Error executing remote Firefox");
  } else {
    if(m_browserProcess->exitStatus() != 0) {
      //no firefox instance? Create new one
      m_browserProcess->clearArguments();
      *m_browserProcess  << "firefox";
      *m_browserProcess  << url;
      if(!m_browserProcess->start()) {
        emit sigError("Error executing Firefox");
      } else {
        m_processRunning = true;
      }
    }
  }
}

int FirefoxRequestor::id()
{
  return Settings::EnumBrowser::Firefox;
}


/**************************** Opera **********************************************/

OperaRequestor::OperaRequestor()
  : BrowserRequestor()
{
}
OperaRequestor::~OperaRequestor()
{
}

void OperaRequestor::doRequest(const QString& url)
{
  init();

  QString arg = "openURL(" + url + ")";

  if(m_processRunning) {
    m_browserProcess->detach();
  }

  m_browserProcess->clearArguments();
  *m_browserProcess  << "opera";
  *m_browserProcess  << "-remote";
  *m_browserProcess << arg;


  if(!m_browserProcess->start()) {
    emit sigError("Error executing remote Opera");
  } else {
    m_processRunning = true;
  }
}

int OperaRequestor::id()
{
  return Settings::EnumBrowser::Opera;
}

#include "dbgrequestor.moc"
