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


#ifndef DBGRECEIVER_H
#define DBGRECEIVER_H
#include <qobject.h>

class DBGHeader;
class DBGFrame;
class DBGResponseTag;
class DBGPack;
class QSocket;
class DBGNet;

class DBGReceiver : public QObject
{
Q_OBJECT
public:
    DBGReceiver(DBGNet* net, const char *name = 0);
    ~DBGReceiver();

    void setSocket(QSocket* socket);
    void clear();
    void readBuffer();
signals:
  void sigError(const QString&);

private slots:
  void slotReadBuffer();

private:
  void            syncBuffer();
  DBGHeader*      readHeader();
  DBGFrame*       readFrame();
  DBGResponseTag* buildTag(int frameName, char* buffer);
  char*           readData(long len);
  void            dispatch(DBGPack* pack);

  QSocket    *m_socket;
  DBGNet     *m_net;
};

#endif
