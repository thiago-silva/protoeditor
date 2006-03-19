/***************************************************************************
 *   Copyright (C) 2004-2005 by Thiago Silva                                    *
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

#ifndef DBGPROFILEDATA_H
#define DBGPROFILEDATA_H
#include <qstring.h>

class DBGProfileLocation;

class DBGProfileData
{
public:
  DBGProfileData();
  DBGProfileData(DBGProfileLocation*, int, double, double, double, double);
  DBGProfileData(int, const QString&, int, const QString&, int, int, double, double, double, double);
  ~DBGProfileData();

  void setLocation(DBGProfileLocation*);
  void setHitCount(int);
  void setTimeAvgHit(double);
  void setTotalTime(double);
  void setMinTime(double);
  void setMaxTime(double);

  DBGProfileLocation* location();
  int hitCount() const;
  double timeAvgHit() const;
  double totalTime() const;
  double minTime() const;
  double maxTime() const;
private:
  DBGProfileLocation* m_location;
  int    m_hitCount;
  double m_timeAvgHit;
  double m_totalTime;
  double m_minTime;
  double m_maxTime;
};
#endif
