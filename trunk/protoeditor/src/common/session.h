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

#ifndef SESSION_H
#define SESSION_H

#include <qobject.h>
#include <qstringlist.h> 

class KProcess;
class DCOPClient;
class QHttp;
class KURL;

class Console;
class IHTTPRequestor;

class Session : public QObject
{
  Q_OBJECT
  public:
    Session();
    ~Session();

    void startRemote(const KURL& url);
    void startLocal(const QString& interpreterCmd, const KURL& url, const QString& args, const QStringList& env = QStringList());

  signals:
    void sigError(const QString& error);

  private:    
    IHTTPRequestor *m_httpRequestor;
    Console        *m_console;
};

class ExternalApp : public QObject
{
  Q_OBJECT
  public:
    ExternalApp(Session*);
    virtual ~ExternalApp();

  signals:
    void sigError(const QString&);

  protected slots:
    void slotProcessExited(KProcess*);

  protected:
    void init();

    KProcess         *m_process;
    bool              m_processRunning;
};

class Console : public ExternalApp
{
  Q_OBJECT
  public:
    Console(Session*);
    ~Console();

    void execute(const QString& interpreterCmd, const KURL& url, const QString& args, const QStringList& env);
};

class IHTTPRequestor : public ExternalApp
{
  public:
    IHTTPRequestor(Session* s) : ExternalApp(s) {};
    virtual ~IHTTPRequestor() {};

    virtual void doRequest(const KURL&) = 0;
};

class DirectHTTP : public IHTTPRequestor
{
  Q_OBJECT
  public:
    DirectHTTP(Session*);
    ~DirectHTTP();
    void doRequest(const KURL&);

  private slots:
    void slotHttpDone(bool);
  private:
    void init();

    QHttp                *m_http;
};

class Browser : public IHTTPRequestor
{
  Q_OBJECT
  public:
    Browser(Session*);
    virtual ~Browser();

    static Browser* retrieveBrowser(int, Session*);    
    virtual int  id() = 0;

  protected:
    static Browser   *m_browser;
};

class KonquerorRequestor : public Browser
{
  Q_OBJECT
  public:
    KonquerorRequestor(Session*);
    ~KonquerorRequestor();
    virtual void doRequest(const KURL&);
    virtual int  id();
  private:
    void init();
    void openNewKonqueror(const KURL&);
    DCOPClient *m_dcopClient;
};


class MozillaRequestor : public Browser
{
  Q_OBJECT
  public:
    MozillaRequestor(Session*);
    ~MozillaRequestor();
    virtual void doRequest(const KURL&);
    virtual int  id();
};

class FirefoxRequestor : public Browser
{
  Q_OBJECT
  public:
    FirefoxRequestor(Session*);
    ~FirefoxRequestor();
    virtual void doRequest(const KURL&);
    virtual int  id();
};

class OperaRequestor : public Browser
{
  Q_OBJECT
  public:
    OperaRequestor(Session*);
    ~OperaRequestor();
    virtual void doRequest(const KURL&);
    virtual int  id();
};


#endif
