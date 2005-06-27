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

#include "httpsession.h"
#include "protoeditorsettings.h"
#include "extoutputsettings.h"
#include <qhttp.h>
#include <kurl.h>
#include <kapplication.h>
#include <kprocess.h>
#include <dcopclient.h>
#include <kdebug.h>
#include <klocale.h>

Browser::Browser()
  : m_http(0), m_browserRequestor(0)
{}

Browser::~Browser()
{
  delete m_browserRequestor;
  delete m_http;
}

void Browser::request(const KURL& url)
{
  if(ProtoeditorSettings::self()->extOutputSettings()->useExternalBrowser()) {
    doBrowserRequest(url);
  } else {
    doHTTPRequest(url);
  }
}

void Browser::doHTTPRequest(const KURL& url)
{
  initHTTPCommunication();

  kdDebug() << "HTTP request \""
      << url.protocol()
      << "://"
      << url.host()
      << ":"
      << url.port()
      << url.path() + url.query()
      << "\""
      << endl;

  m_http->setHost(url.host(), url.port());
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
  if(error && (m_http->error() != QHttp::Aborted)) {
    emit sigError(i18n("HTTP Conection error: " + m_http->errorString()));
  }
  m_http->abort();
  kdDebug() << "HTTP closed connection!" << endl;
}

void Browser::doBrowserRequest(const KURL& url)
{
  m_browserRequestor = BrowserRequestor::retrieveBrowser(
      ProtoeditorSettings::self()->extOutputSettings()->browser(), this);
  //TODO: assert-me
  m_browserRequestor->doRequest(url.prettyURL());
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
    case ExtOutputSettings::EnumBrowser::Konqueror:
      if(m_browserRequestor && (m_browserRequestor->id() == ExtOutputSettings::EnumBrowser::Konqueror)) {
        return m_browserRequestor;
      }
      delete m_browserRequestor;
      m_browserRequestor = new KonquerorRequestor();
      connect(m_browserRequestor, SIGNAL(sigError(const QString&)), br,
              SIGNAL(sigError(const QString&)));
      break;
    case ExtOutputSettings::EnumBrowser::Mozilla:
      if(m_browserRequestor && (m_browserRequestor->id() == ExtOutputSettings::EnumBrowser::Mozilla)) {
        return m_browserRequestor;
      }
      delete m_browserRequestor;
      m_browserRequestor = new MozillaRequestor();
      connect(m_browserRequestor, SIGNAL(sigError(const QString&)), br,
              SIGNAL(sigError(const QString&)));
      break;
    case ExtOutputSettings::EnumBrowser::Firefox:
      if(m_browserRequestor && (m_browserRequestor->id() == ExtOutputSettings::EnumBrowser::Firefox)) {
        return m_browserRequestor;
      }
      delete m_browserRequestor;
      m_browserRequestor = new FirefoxRequestor();
      connect(m_browserRequestor, SIGNAL(sigError(const QString&)), br,
              SIGNAL(sigError(const QString&)));
      break;
    case ExtOutputSettings::EnumBrowser::Opera:
      if(m_browserRequestor && (m_browserRequestor->id() == ExtOutputSettings::EnumBrowser::Opera)) {
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
  return ExtOutputSettings::EnumBrowser::Konqueror;
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
  return ExtOutputSettings::EnumBrowser::Mozilla;
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
  return ExtOutputSettings::EnumBrowser::Firefox;
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
  return ExtOutputSettings::EnumBrowser::Opera;
}


#include "httpsession.moc"
