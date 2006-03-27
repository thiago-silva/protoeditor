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

#include "dbgprofiledata.h"
#include "dbgprofilelocation.h"

DBGProfileData::DBGProfileData()
    : m_location(0), m_hitCount(0), m_timeAvgHit(0), m_totalTime(0), m_minTime(0), m_maxTime(0)

{}

DBGProfileData::DBGProfileData(DBGProfileLocation* location,
                               int hitCount, double timeAvgHit, double totalTime,
                               double minTime, double maxTime)
    : m_location(location),
    m_hitCount(hitCount),
    m_timeAvgHit(timeAvgHit),
    m_totalTime(totalTime),
    m_minTime(minTime),
    m_maxTime(maxTime)
{}

DBGProfileData::DBGProfileData(int modid, const QString& modName,
                               int ctxId, const QString& ctxName,
                               int line, int hitCount, double timeAvgHit,
                               double totalTime, double minTime, double maxTime)
  : m_hitCount(hitCount),
    m_timeAvgHit(timeAvgHit),
    m_totalTime(totalTime),
    m_minTime(minTime),
    m_maxTime(maxTime)
{
  m_location = new DBGProfileLocation(modid, modName, ctxId, ctxName, line);
}

DBGProfileData::~DBGProfileData()
{}

void DBGProfileData::setLocation(DBGProfileLocation* location)
{
  m_location = location;
}

void DBGProfileData::setHitCount(int hitCount)
{
  m_hitCount = hitCount;
}

void DBGProfileData::setTimeAvgHit(double timeAvgHit)
{
  m_timeAvgHit = timeAvgHit;
}

void DBGProfileData::setTotalTime(double totalTime)
{
  m_totalTime = totalTime;
}

void DBGProfileData::setMinTime(double minTime)
{
  m_minTime = minTime;
}

void DBGProfileData::setMaxTime(double maxTime)
{
  m_maxTime = maxTime;
}

DBGProfileLocation* DBGProfileData::location()
{
  return m_location;
}

int DBGProfileData::hitCount() const
{
  return m_hitCount;
}

double DBGProfileData::timeAvgHit() const
{
  return m_timeAvgHit;
}

double DBGProfileData::totalTime() const
{
  return m_totalTime;
}

double DBGProfileData::minTime() const
{
  return m_minTime;
}
double DBGProfileData::maxTime() const
{
  return m_maxTime;
}


