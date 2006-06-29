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

class ExternalApp;
class Browser;

class Session : public QObject
{
  Q_OBJECT
  public:
    ~Session();

    static void dispose();
    static Session* self();
    
    void startRemote(const KURL& url);
    void startLocal(const QString& lang, const KURL& url, const QString& args, const QStringList& env = QStringList());

  signals:
    void sigError(const QString& error);

  private slots:
    void slotHttpDone(bool);
  private:
    Session();
    void doHTTPRequest(const KURL& url);
    void doExternalRequest(const KURL&);    
    void initHTTPCommunication();

    static Session* m_self;

    QHttp                *m_http;
    ExternalApp          *m_externalApp;
};

class ExternalApp : public QObject
{
  Q_OBJECT
  public:
    ExternalApp();
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
    Console();
    ~Console();

    void execute(const QString& lang, const KURL& url, const QString& args, const QStringList& env);
};

class Browser : public ExternalApp
{
  Q_OBJECT
  public:
    Browser();
    virtual ~Browser();

    static Browser* retrieveBrowser(int, Session*);

    virtual void doRequest(const KURL&) = 0;
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
