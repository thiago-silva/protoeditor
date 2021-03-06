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

#ifndef DBGCONFIGURATION_H
#define DBGCONFIGURATION_H

#include <qstring.h>

class DBGConfiguration{
public:
  DBGConfiguration(const QString& localBaseDir, const QString& serverBaseDir,
                   int listenPort, const QString& host);

  ~DBGConfiguration();

  void setLocalBaseDir(const QString&);
  void setServerBaseDir(const QString&);
  void setListenPort(int);
  void setServerHost(const QString&);

  const QString& localBaseDir() const;
  const QString& serverBaseDir() const;
  int   listenPort() const;
  const QString& serverHost() const;

private:
  QString m_localBaseDir;
  QString m_serverBaseDir;
  int     m_listenPort;
  QString m_serverHost;
};

#endif
