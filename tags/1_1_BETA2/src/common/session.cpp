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

#include "session.h"
#include "protoeditor.h"
#include "protoeditorsettings.h"
#include "extappsettings.h"

#include <qhttp.h>
#include <kurl.h>
#include <kapplication.h>
#include <kprocess.h>
#include <dcopclient.h>
#include <kdebug.h>
#include <klocale.h>


Session::Session()
    : m_httpRequestor(0), m_console(0)
{}

Session::~Session()
{
  delete m_httpRequestor;
  delete m_console;
}

void Session::startRemote(const KURL& url)
{
  if(Protoeditor::self()->settings()->extAppSettings()->useExternalBrowser())
  {
    m_httpRequestor = Browser::retrieveBrowser(
                          Protoeditor::self()->settings()->extAppSettings()->externalBrowser(), this);
  }
  else
  {
    delete m_httpRequestor;
    m_httpRequestor = new DirectHTTP(this);
  }
   m_httpRequestor->doRequest(url);
}

void Session::startLocal(const QString& interpreterCmd, const KURL& url, const QString& args, const QStringList& env)
{
  delete m_console;

  m_console = new Console(this);

  m_console->execute(interpreterCmd, url, args, env);
}

/************************* ExternalApp **************************************************/

ExternalApp::ExternalApp(Session* session)
  : m_process(0), m_processRunning(false)
{
  connect(this, SIGNAL(sigError(const QString&)), session,
          SIGNAL(sigError(const QString&)));
}

ExternalApp::~ExternalApp()
{
  if(m_process)
  {
    m_process->kill();
  }

  delete m_process;
}


void ExternalApp::slotProcessExited(KProcess*)
{
  m_processRunning = false;
}

void ExternalApp::init()
{
  if(!m_processRunning)
  {    
    delete m_process;

    m_process = new KProcess;
    connect(m_process, SIGNAL(processExited(KProcess*)),
            this, SLOT(slotProcessExited(KProcess*)));
  }
}

/***************************** Console *****************************************/

Console::Console(Session* session)
 :  ExternalApp(session)
{

}

Console::~Console()
{
}

void Console::execute(const QString& interpreterCmd, const KURL& url, const QString& args, const QStringList& env)
{
  init();

  if(m_processRunning)
  {
    m_process->detach();
  }

  m_process->clearArguments();

  QString consoleApp = Protoeditor::self()->settings()->extAppSettings()->console();

  QStringList::const_iterator it = env.begin();
  QString name;
  QString val;
  QString verbose;
  while(it != env.end()) {
    name = (*it);
    ++it;
    val = KProcess::quote(*it);
    ++it;
    verbose += name + "=" + val + " ";
  }

  if(Protoeditor::self()->settings()->extAppSettings()->useConsole()) 
  {
    //use external console

    QString prologue = QString("cd ") +  url.directory() + " && ";

    QString cmd = interpreterCmd + " " + url.path() + " " + args + "; echo " +
                i18n("'Press Enter to continue...'") + "; read";

    kdDebug() << "executing console: " << (consoleApp + " /bin/sh") << " -c "              
              << prologue + verbose + cmd << endl;

    *m_process << QStringList::split(' ',consoleApp + " /bin/sh") << "-c"
      << (prologue + verbose + cmd);
  }
  else
  {
    //no terminal
    kdDebug() << "executing : " << "/bin/sh" << " -c "
              << QString("cd ") <<  url.directory() << ";"
              << (interpreterCmd + " " + url.path() + " " + args + ";echo " +
                 i18n("'Press Enter to continue...'") + ";read")
              << endl;
  
    *m_process << "/bin/sh" << "-c" << 
      (QString("cd ") +  url.directory() + ";" + interpreterCmd + " " + url.path() + " " + args);
  }

  if(!m_process->start())
  {
    emit sigError(i18n("Error executing console."));
  }
  else
  {
    m_processRunning = true;
  }
}


/************************* DirectHTTP ********************************/

DirectHTTP::DirectHTTP(Session* session)
  : IHTTPRequestor(session), m_http(0)
{
}

DirectHTTP::~DirectHTTP()
{
}

void DirectHTTP::doRequest(const KURL& url)
{
  init();

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

void DirectHTTP::init()
{
  ExternalApp::init();

  if(!m_http)
  {
    m_http = new QHttp;
    connect(m_http, SIGNAL(done(bool)), this, SLOT(slotHttpDone(bool)));
  }
}

void DirectHTTP::slotHttpDone(bool error)
{
  if(error && (m_http->error() != QHttp::Aborted))
  {
    emit sigError(i18n("HTTP Conection error: " + m_http->errorString()));
  }
  m_http->abort();
  kdDebug() << "HTTP closed connection!" << endl;
}

/***************************** Browser *****************************************/

Browser* Browser::m_browser = 0;

Browser::Browser(Session* session)
    : IHTTPRequestor(session)
{}

Browser::~Browser()
{

}

Browser* Browser::retrieveBrowser(int browserno, Session* session)
{
  switch(browserno)
  {
    case ExtAppSettings::EnumBrowser::Konqueror:
      if(m_browser && (m_browser->id() == ExtAppSettings::EnumBrowser::Konqueror))
      {
        return m_browser;
      }
      delete m_browser;
      m_browser = new KonquerorRequestor(session);
      break;
    case ExtAppSettings::EnumBrowser::Mozilla:
      if(m_browser && (m_browser->id() == ExtAppSettings::EnumBrowser::Mozilla))
      {
        return m_browser;
      }
      delete m_browser;
      m_browser = new MozillaRequestor(session);
      break;
    case ExtAppSettings::EnumBrowser::Firefox:
      if(m_browser && (m_browser->id() == ExtAppSettings::EnumBrowser::Firefox))
      {
        return m_browser;
      }
      delete m_browser;
      m_browser = new FirefoxRequestor(session);
      break;
    case ExtAppSettings::EnumBrowser::Opera:
      if(m_browser && (m_browser->id() == ExtAppSettings::EnumBrowser::Opera))
      {
        return m_browser;
      }
      delete m_browser;
      m_browser = new OperaRequestor(session);
      break;
  }

  return m_browser;
}


/***************************** KONQUEROR *****************************************/

KonquerorRequestor::KonquerorRequestor(Session* session)
    : Browser(session), m_dcopClient(0)
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
    emit sigError(i18n("Error opening browser"));
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
    emit sigError(i18n("Error opening Konqueror."));
  }
  else
  {
    kdDebug() << "New konqueror PID: " << m_process->pid() << endl;
    m_processRunning = true;
  }
}

int KonquerorRequestor::id()
{
  return ExtAppSettings::EnumBrowser::Konqueror;
}

void KonquerorRequestor::init()
{
  Browser::init();

  if(!m_dcopClient)
  {
    m_dcopClient = KApplication::dcopClient();

    if(!m_dcopClient->attach())
    {
      emit sigError(i18n("Could not init external application."));
    }
  }
}


/******************* MOZILLA *********************************/
MozillaRequestor::MozillaRequestor(Session* session)
    : Browser(session)
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
    emit sigError(i18n("Error executing remote Mozilla."));
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
        emit sigError(i18n("Error executing Mozilla."));
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
  return ExtAppSettings::EnumBrowser::Mozilla;
}


/************************ FIREFOX ***********************************************/
FirefoxRequestor::FirefoxRequestor(Session* session) 
    : Browser(session)
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
    emit sigError(i18n("Error executing Firefox."));
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
        emit sigError(i18n("Error executing Firefox."));
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
  return ExtAppSettings::EnumBrowser::Firefox;
}


/**************************** Opera **********************************************/

OperaRequestor::OperaRequestor(Session* session)
    : Browser(session)
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
    emit sigError(i18n("Error executing Opera."));
  }
  else
  {
    m_processRunning = true;
  }
}

int OperaRequestor::id()
{
  return ExtAppSettings::EnumBrowser::Opera;
}

#include "session.moc"
