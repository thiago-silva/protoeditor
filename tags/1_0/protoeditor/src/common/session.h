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
class Browser;


class ExternalAppRequestor;
class ConsoleRequestor;

class Session : public QObject
{
  Q_OBJECT
  public:
    
    ~Session();

    static void dispose();
    static Session* self();
    
    void start(const KURL& url);
    void start(const KURL& url, const QString& args, const QStringList& env = QStringList());

    const QString&     arguments();
    const QStringList& environment();
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
    ExternalAppRequestor *m_extAppRequestor;

    QString     m_args;
    QStringList m_env;
};

class ExternalAppRequestor : public QObject
{
  Q_OBJECT
  public:
    ExternalAppRequestor();
    virtual ~ExternalAppRequestor();

    static ExternalAppRequestor* retrieveExternalApp(int, Session*);

    virtual void doRequest(const KURL&) = 0;
    virtual int  id() = 0;
  signals:
    void sigError(const QString&);

  protected:
    void init();

    static ExternalAppRequestor *m_extAppRequestor;

    KProcess         *m_process;
    bool              m_processRunning;

  private slots:
    void slotProcessExited(KProcess*);
};

class KonquerorRequestor : public ExternalAppRequestor
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


class MozillaRequestor : public ExternalAppRequestor
{
  Q_OBJECT
  public:
    MozillaRequestor(Session*);
    ~MozillaRequestor();
    virtual void doRequest(const KURL&);
    virtual int  id();
};

class FirefoxRequestor : public ExternalAppRequestor
{
  Q_OBJECT
  public:
    FirefoxRequestor(Session*);
    ~FirefoxRequestor();
    virtual void doRequest(const KURL&);
    virtual int  id();
};

class OperaRequestor : public ExternalAppRequestor
{
  Q_OBJECT
  public:
    OperaRequestor(Session*);
    ~OperaRequestor();
    virtual void doRequest(const KURL&);
    virtual int  id();
};

class ConsoleRequestor : public ExternalAppRequestor
{
  Q_OBJECT
  public:
    ConsoleRequestor(Session*);
    ~ConsoleRequestor();
    virtual void doRequest(const KURL&);
    virtual int  id();

  private:
    QString     m_args;
    QStringList m_env;
};


#endif
