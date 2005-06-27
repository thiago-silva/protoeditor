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

#ifndef HTTPSESSION_H
#define HTTPSESSION_H

#include <qobject.h>

class KProcess;
class DCOPClient;
class QHttp;
class KURL;
class Browser;

/*
  All classes here are used to make an http request (to trigger the server side debugger).
  Options are:
    -direct http request (via QHttp)
    -Browser request. Supported browsers are:
      +Konqueror (DCOP)
      +Mozilla (command line)
      +FireFox (command line)
      +Opera (command line)
*/

class BrowserRequestor : public QObject
{
  Q_OBJECT
  public:
    BrowserRequestor();
    virtual ~BrowserRequestor();

    static BrowserRequestor* retrieveBrowser(int, Browser*);

    virtual void doRequest(const QString&) = 0;
    virtual int  id() = 0;
  signals:
    void sigError(const QString&);

  protected:
    void init();

    static BrowserRequestor *m_browserRequestor;

    KProcess         *m_browserProcess;
    bool              m_processRunning;

  private slots:
    void slotProcessExited(KProcess*);
};

class KonquerorRequestor : public BrowserRequestor
{
  Q_OBJECT
  public:
    KonquerorRequestor();
    ~KonquerorRequestor();
    virtual void doRequest(const QString&);
    virtual int  id();
  private:
    void init();
    void openNewKonqueror(const QString&);
    DCOPClient *m_dcopClient;
};


class MozillaRequestor : public BrowserRequestor
{
  Q_OBJECT
  public:
    MozillaRequestor();
    ~MozillaRequestor();
    virtual void doRequest(const QString&);
    virtual int  id();
};

class FirefoxRequestor : public BrowserRequestor
{
  Q_OBJECT
  public:
    FirefoxRequestor();
    ~FirefoxRequestor();
    virtual void doRequest(const QString&);
    virtual int  id();
};

class OperaRequestor : public BrowserRequestor
{
  Q_OBJECT
  public:
    OperaRequestor();
    ~OperaRequestor();
    virtual void doRequest(const QString&);
    virtual int  id();
};

/**************************************************************/

class Browser : public QObject
{
  Q_OBJECT
  public:
    Browser();
    ~Browser();
    void request(const KURL&);

  signals:
    void sigError(const QString& error);

  private slots:
    void slotHttpDone(bool);
  private:
    void doHTTPRequest(const KURL& url);
    void doBrowserRequest(const KURL&);
    void initHTTPCommunication();

    QHttp            *m_http;
    BrowserRequestor *m_browserRequestor;
};

#endif
