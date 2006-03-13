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

#include "gbnet.h"
#include "debuggergb.h"

#include "connection.h"
#include "sitesettings.h"
#include "gbsettings.h"
#include "session.h"

#include <qsocket.h>
#include <kdebug.h>
#include <kurl.h>
#include <qregexp.h>


GBNet::GBNet(DebuggerGB* debugger, QObject *parent, const char *name)
    : QObject(parent, name), m_debugger(debugger), m_con(0), m_socket(0)
{
  m_con = new Connection();
  connect(m_con, SIGNAL(sigAccepted(QSocket*)), this, SLOT(slotIncomingConnection(QSocket*)));
  connect(m_con, SIGNAL(sigClientClosed()), this, SLOT(slotGBClosed()));
  connect(m_con, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));
}

GBNet::~GBNet()
{
  delete m_con;
}

bool GBNet::startListener(int port)
{
  return m_con->listenOn(port);
}

void GBNet::stopListener()
{
  m_con->close();
}


void GBNet::startDebugging(const QString& filePath, SiteSettings* site)
{
  //translate remote/local path
  QString uri = filePath;
  uri = uri.remove(0, site->localBaseDir().length());

  makeHttpRequest(site->url(), uri);
}

void GBNet::requestContinue()
{}

void GBNet::requestStop()
{
  //
  m_con->closeClient();
}

void GBNet::requestStepInto()
{}

void GBNet::requestStepOver()
{}

void GBNet::requestStepOut()
{}


void GBNet::sendRunMode(int)
{
  /*
  switch(mode)
  {
  case GBSettings::PauseMode:
  m_net->sendCommand("pause");
  break;
  case GBSettings::RunMode:
  m_net->sendCommand("run");
  break;
  case GBSettings::TraceMode:
  m_net->sendCommand("trace");
  break;
  default:
  emit sigInternalError("Unknow execution mode");
  m_net->sendCommand("pause");
  }
  */

  //set hardcoded default runmode to pause
  sendCommand("pause");
}

void GBNet::sendErrorSettings(int errorno)
{
  QMap<QString,QString> data;
  data["errormask"] =   QString::number(errorno);
  sendCommand("seterrormask",data);
}

void GBNet::sendHaveSource(bool have, const QString& file)
{
    //assuming we have the source
  QMap<QString,QString> data;
  data["filename"] = file;
  if(have)
  {
    sendCommand("havesource", data);
  }
  else
  {
    sendCommand("sendsource", data);
  }
}

void GBNet::requestBacktrace()
{
  sendCommand("sendbacktrace");
}

void GBNet::sendWait()
{
  sendCommand("wait");
}

void GBNet::sendNext()
{
  sendCommand("next");
}

void GBNet::slotIncomingConnection(QSocket* socket)
{
  connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadBuffer()));
  m_socket = socket;

  emit sigGBStarted();

  sendCommand("wait");
  sendCommand("next");
  sendCommand("next");
  //sendCommand("sendbacktrace");
}

void GBNet::slotGBClosed()
{
  emit sigGBClosed();
}

void GBNet::slotError(const QString& msg)
{
  error(msg);
}


QString GBNet::readSocket()
{
  int size = m_socket->bytesAvailable();
  char buffer[size];
  int read;

  QString str;
  QString text;
  do
  {
    read = m_socket->readBlock(buffer, size);
    str.setAscii(buffer,read);
    text += str;
  }
  while(m_socket->bytesAvailable());
  return text;
}

void GBNet::slotReadBuffer()
{
  int pos;

  QString str;
  QString text;

  while(true)
  {
  beginWhile:
    text += readSocket();

    do
    {
      pos  = text.find(':');

      if(text.find(':', pos+1) == -1) //search for the next :
      {
        m_socket->waitForMore(-1);
        goto beginWhile;
      }
      else
      {
        QString name = text.left(pos);

        QRegExp rx;
        rx.setPattern(":(\\d*);");
        if(rx.search(text, pos) == -1)
        {
          error("Error parsing network data");
          return;
        }

        int dataSize = rx.cap(1).toInt();
        pos += rx.matchedLength();

        uint total = pos + dataSize;
        if(total > text.length())
        {
          m_socket->waitForMore(-1);
          goto beginWhile;
        }
        else
        {
          QString data = text.mid(pos, dataSize);
          m_debugger->processInput(name, unserialize(data));
          text.remove(0, total);
        }
      }
    }
    while(text.length());
    break;
  }
}

void GBNet::makeHttpRequest(const QString& _url, const QString& path)
{
  KURL url(_url);
  url.setPath(url.path() + m_debugger->settings()->startSessionScript());
  url.setQuery(QString("gbdScript=") + path);

  kdDebug() << url << endl;
  Session::self()->start(url);
}

void GBNet::error(const QString& msg)
{
  emit sigError(msg);
  m_con->closeClient();
}

void GBNet::sendCommand(const QString& cmd, const StringMap_t& data)
{
  QString serialData = serialize(data);

  QString buffer;
  if(data.size())
  {
    buffer = cmd + ":" + QString::number(serialData.length()) + ";";
    buffer += serialData;
  }
  else
  {
    buffer = cmd + ":0;";
  }


  //QString s = "sendactiveline:5;a:0{}";
  m_socket->writeBlock(buffer, buffer.length());
}

QString GBNet::serialize(const StringMap_t& data)
{
  QString serialData = "a:" + QString::number(data.size()) + ":{";
  StringMap_t::const_iterator it;

  bool ok;
  for ( it = data.begin(); it != data.end(); ++it )
  {
    ok = it.data().toInt(&ok);
    if(ok && !it.data().isEmpty())
    {
      serialData += "s:" + QString::number(it.key().length()) + ":";
      serialData += "\"" + it.key() + "\";";
      serialData += "i:" + it.data() + ";";
    }
    else
    {
      serialData += "s:" + QString::number(it.key().length()) + ":";
      serialData += "\"" + it.key() + "\";s:";
      serialData += QString::number(it.data().length()) + ":\"" + it.data() + "\";";
    }
  }

  serialData += "}";
  return serialData;
}

GBNet::StringMap_t GBNet::unserialize(const QString& text)
{
  //a:2:{s:4:\"line\";i:0;s:8:\"filename\";s:33:\"/usr/local/apache/htdocs/text.php\";}

  StringMap_t map;
  int idx = 0;
  
  if(text.length() == 0) return map;
          
  QRegExp rx;
  rx.setPattern("a:(\\d*):\\{");
  if(rx.search(text, idx) == -1)
  {
    error("Error parsing network data");
    return map;
  }

  int fields = rx.cap(1).toInt();
  idx += rx.matchedLength();

  for(int i = 0; i < fields; i++)
  {
    QString name = getField(&idx, text);
    QString value = getField(&idx, text);
    kdDebug() << value << endl;
    map[name] = value;
  }
  return map;
}

QString GBNet::getField(int* idx, QString text)
{
  //s:4:\"line\";i:0;s:8:\"filename\";s:33:\"/usr/local/apache/htdocs/text.php\";}
  if(text[*idx] == 's')
  {
    QRegExp rx;
    rx.setPattern("s:(\\d*):");
    if(rx.search(text, *idx) == -1)
    {
      error("Error parsing network data");
      return QString::null;
    }
    
    int len = rx.cap(1).toInt();
    *idx += rx.matchedLength(); //4
  
    int begin = *idx + 1;
  
    *idx  += len + 3; //\";
    return text.mid(begin, len);
  }
  else if(text[*idx] == 'i')
  {
    QRegExp rx;
    rx.setPattern("i:(\\d*);");
    if(rx.search(text, *idx) == -1)
    {
      error("Error parsing network data");
      return QString::null;
    }
    
    QString num = rx.cap(1);
    *idx += rx.matchedLength();
  
    return num;
  }

  return QString::null;
}

#include "gbnet.moc"
