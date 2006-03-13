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
#include "phpsettings.h"

#include <qhttp.h>
#include <kurl.h>
#include <kapplication.h>
#include <kprocess.h>
#include <dcopclient.h>
#include <kdebug.h>
#include <klocale.h>


AppSession* AppSession::m_self = 0;

AppSession::AppSession()
    : m_http(0), m_extAppRequestor(0)
{}

AppSession::~AppSession()
{
  delete m_extAppRequestor;
  delete m_http;
}

void AppSession::dispose()
{
  delete AppSession::m_self;
}

AppSession* AppSession::self()  
{
  if(!AppSession::m_self)
  {
    AppSession::m_self = new AppSession();
  }
  return m_self;
}

void AppSession::start(const KURL& url, bool forceConsoleMode)
{
  if(forceConsoleMode)
  {
    m_extAppRequestor = ExternalAppRequestor::retrieveExternalApp(ExtAppSettings::EnumExtApp::Console, this);
    m_extAppRequestor->doRequest(url);
  }
  else if(ProtoeditorSettings::self()->extAppSettings()->useExternalApp())
  {
    doExternalRequest(url);
  }
  else
  {
    doHTTPRequest(url);
  }
  m_env.clear();
}

void AppSession::start(const KURL& url, const QStringList& env, bool forceConsoleMode)
{
  m_env = env;
  start(url, forceConsoleMode);
}

void AppSession::initHTTPCommunication()
{
  if(!m_http)
  {
    m_http = new QHttp;
    connect(m_http, SIGNAL(done(bool)), this, SLOT(slotHttpDone(bool)));
  }
}

void AppSession::doHTTPRequest(const KURL& url)
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

void AppSession::slotHttpDone(bool error)
{
  if(error && (m_http->error() != QHttp::Aborted))
  {
    emit sigError(i18n("HTTP Conection error: " + m_http->errorString()));
  }
  m_http->abort();
  kdDebug() << "HTTP closed connection!" << endl;
}

void AppSession::doExternalRequest(const KURL& url)
{
  m_extAppRequestor = ExternalAppRequestor::retrieveExternalApp(
                        ProtoeditorSettings::self()->extAppSettings()->externalApp(), this);

  m_extAppRequestor->doRequest(url);
}

const QStringList& AppSession::environment()
{
  return m_env;
}

/***************************** ExternalAppRequestor *****************************************/

ExternalAppRequestor* ExternalAppRequestor::m_extAppRequestor = 0;

ExternalAppRequestor::ExternalAppRequestor()
    : m_process(0), m_processRunning(false)

{}

ExternalAppRequestor::~ExternalAppRequestor()
{
  delete m_process;
}

ExternalAppRequestor* ExternalAppRequestor::retrieveExternalApp(int extApp, AppSession* session)
{
  switch(extApp)
  {
    case ExtAppSettings::EnumExtApp::Konqueror:
      if(m_extAppRequestor && (m_extAppRequestor->id() == ExtAppSettings::EnumExtApp::Konqueror))
      {
        return m_extAppRequestor;
      }
      delete m_extAppRequestor;
      m_extAppRequestor = new KonquerorRequestor(session);
      break;
    case ExtAppSettings::EnumExtApp::Mozilla:
      if(m_extAppRequestor && (m_extAppRequestor->id() == ExtAppSettings::EnumExtApp::Mozilla))
      {
        return m_extAppRequestor;
      }
      delete m_extAppRequestor;
      m_extAppRequestor = new MozillaRequestor(session);
      break;
    case ExtAppSettings::EnumExtApp::Firefox:
      if(m_extAppRequestor && (m_extAppRequestor->id() == ExtAppSettings::EnumExtApp::Firefox))
      {
        return m_extAppRequestor;
      }
      delete m_extAppRequestor;
      m_extAppRequestor = new FirefoxRequestor(session);
      break;
    case ExtAppSettings::EnumExtApp::Opera:
      if(m_extAppRequestor && (m_extAppRequestor->id() == ExtAppSettings::EnumExtApp::Opera))
      {
        return m_extAppRequestor;
      }
      delete m_extAppRequestor;
      m_extAppRequestor = new OperaRequestor(session);
      break;
    case ExtAppSettings::EnumExtApp::Console:
      delete m_extAppRequestor;
      m_extAppRequestor = new ConsoleRequestor(session);
      break;
  }

  connect(m_extAppRequestor, SIGNAL(sigError(const QString&)), session,
          SIGNAL(sigError(const QString&)));

  return m_extAppRequestor;
}

void ExternalAppRequestor::init()
{
  if(!m_processRunning)
  {

    delete m_process;

    m_process = new KProcess;
    connect(m_process, SIGNAL(processExited(KProcess*)),
            this, SLOT(slotProcessExited(KProcess*)));
  }
}

void ExternalAppRequestor::slotProcessExited(KProcess*)
{
  m_processRunning = false;
}


/***************************** KONQUEROR *****************************************/

KonquerorRequestor::KonquerorRequestor(AppSession*)
    : ExternalAppRequestor(), m_dcopClient(0)
{}
KonquerorRequestor::~KonquerorRequestor()
{}

void KonquerorRequestor::doRequest(const KURL& url)
{
  init();

  if(!m_processRunning)
  {
    openNewKonqueror(url);
    return;
  }

  QByteArray data;
  QDataStream arg(data, IO_WriteOnly);
  arg << url.prettyURL();

  kdDebug() << "asking for konqueror ("
  << m_process->pid()
  << ") to open URL: "
  << url
  << endl;

  QString cmd = "konqueror";

  if(!m_dcopClient->send(
        (cmd + "-" + QString::number(m_process->pid())).ascii()
        , "konqueror-mainwindow#1", "openURL(QString)"
        , data))
  {
    emit sigError("Error opening browser");
  }
}

void KonquerorRequestor::openNewKonqueror(const KURL& url)
{
  QString cmd = "konqueror";
  *m_process << cmd;
  *m_process << url.prettyURL();

  kdDebug() << "creating new Konqueror window for: " << url.prettyURL() << endl;

  if(!m_process->start())
  {
    emit sigError("Error opening Konqueror.");
  }
  else
  {
    kdDebug() << "New konqueror PID: " << m_process->pid() << endl;
    m_processRunning = true;
  }
}

int KonquerorRequestor::id()
{
  return ExtAppSettings::EnumExtApp::Konqueror;
}

void KonquerorRequestor::init()
{
  ExternalAppRequestor::init();

  if(!m_dcopClient)
  {
    m_dcopClient = KApplication::dcopClient();

    if(!m_dcopClient->attach())
    {
      emit sigError("Could not init external application.");
    }
  }
}


/******************* MOZILLA *********************************/
MozillaRequestor::MozillaRequestor(AppSession*)
    : ExternalAppRequestor()
{}
MozillaRequestor::~MozillaRequestor()
{}

void MozillaRequestor::doRequest(const KURL& url)
{
  init();

  QString arg = "openURL(" + url.prettyURL() + ")";

  if(m_processRunning)
  {
    m_process->detach();
  }

  kdDebug() << "executing \"" << "mozilla -remote " << arg << "\"\n";

  m_process->clearArguments();
  *m_process  << "mozilla";
  *m_process  << "-remote";
  *m_process << arg;


  if(!m_process->start(KProcess::Block))
  {
    emit sigError("Error executing remote Mozilla.");
  }
  else
  {
    if(m_process->exitStatus() != 0)
    {
      //no mozilla instance? Create new one
      m_process->clearArguments();
      *m_process  << "mozilla";
      *m_process  << url.prettyURL();
      if(!m_process->start())
      {
        emit sigError("Error executing Mozilla.");
      }
      else
      {
        m_processRunning = true;
      }
    }
  }
}

int MozillaRequestor::id()
{
  return ExtAppSettings::EnumExtApp::Mozilla;
}


/************************ FIREFOX ***********************************************/
FirefoxRequestor::FirefoxRequestor(AppSession*)
    : ExternalAppRequestor()
{}
FirefoxRequestor::~FirefoxRequestor()
{}

void FirefoxRequestor::doRequest(const KURL& url)
{
  init();

  QString arg = "openURL(" + url.prettyURL() + ")";

  if(m_processRunning)
  {
    m_process->detach();
  }

  kdDebug() << "executing \"" << "firefox -remote " << arg << "\"\n";

  m_process->clearArguments();
  *m_process  << "firefox";
  *m_process  << "-remote";
  *m_process << arg;


  if(!m_process->start(KProcess::Block))
  {
    emit sigError("Error executing Firefox.");
  }
  else
  {
    if(m_process->exitStatus() != 0)
    {
      //no firefox instance? Create new one
      m_process->clearArguments();
      *m_process  << "firefox";
      *m_process  << url.prettyURL();
      if(!m_process->start())
      {
        emit sigError("Error executing Firefox.");
      }
      else
      {
        m_processRunning = true;
      }
    }
  }
}

int FirefoxRequestor::id()
{
  return ExtAppSettings::EnumExtApp::Firefox;
}


/**************************** Opera **********************************************/

OperaRequestor::OperaRequestor(AppSession*)
    : ExternalAppRequestor()
{}
OperaRequestor::~OperaRequestor()
{}

void OperaRequestor::doRequest(const KURL& url)
{
  init();

  QString arg = "openURL(" + url.prettyURL() + ")";

  if(m_processRunning)
  {
    m_process->detach();
  }

  kdDebug() << "executing \"" << "opera -remote " << arg << "\"\n";

  m_process->clearArguments();
  *m_process  << "opera";
  *m_process  << "-remote";
  *m_process << arg;


  if(!m_process->start())
  {
    emit sigError("Error executing Opera.");
  }
  else
  {
    m_processRunning = true;
  }
}

int OperaRequestor::id()
{
  return ExtAppSettings::EnumExtApp::Opera;
}

/**************************** Console **********************************************/

ConsoleRequestor::ConsoleRequestor(AppSession* session)
    : ExternalAppRequestor()
{
  m_env = session->environment();
}
ConsoleRequestor::~ConsoleRequestor()
{}

void ConsoleRequestor::doRequest(const KURL& url)
{
  init();

  if(m_processRunning)
  {
    m_process->detach();
  }

  m_process->clearArguments();

  //find the mimetype of filePath, and use the proper interpreter
  QString cmd = ProtoeditorSettings::self()->phpSettings()->PHPCommand();

  QString consoleApp = ProtoeditorSettings::self()->extAppSettings()->console();

  kdDebug() << "executing console: " << consoleApp.arg("/bin/sh") << " -c "
            << m_env.join(" ") << " " 
            << cmd.arg(url.path()) + ";echo \"Press Enter to continue...\";read" << endl;

  QStringList::Iterator it = m_env.begin();
  QString name;
  QString val;
  while(it != m_env.end()) {
    name = (*it);
    ++it;
    val = (*it);
    ++it;
    m_process->setEnvironment(name, val);
  }

  //KProcess::quote(filePath)
  *m_process << QStringList::split(' ',consoleApp.arg("/bin/sh")) << "-c"
    <<  (cmd.arg(url.path()) + ";echo \"Press Enter to continue...\";read");

  if(!m_process->start())
  {
    emit sigError("Error executing console.");
  }
  else
  {
    m_processRunning = true;
  }  
}

int ConsoleRequestor::id()
{
  return ExtAppSettings::EnumExtApp::Console;
}

#include "httpsession.moc"
