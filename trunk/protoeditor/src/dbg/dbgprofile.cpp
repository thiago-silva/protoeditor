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

#include "dbgprofile.h"

#include <klistview.h>
#include <klocale.h>
#include <qlayout.h>

DBGProfileData::DBGProfileData()
    : m_line(0), m_hitCount(0), m_timeAvgHit(0), m_totalTime(0), m_minTime(0), m_maxTime(0)

{}

DBGProfileData::DBGProfileData(const QString& filePath, const QString& context, int line,
                               int hitCount, double timeAvgHit, double totalTime,
                               double minTime, double maxTime)
    : m_line(line), m_hitCount(hitCount), m_timeAvgHit(timeAvgHit), m_totalTime(totalTime),
    m_minTime(minTime), m_maxTime(maxTime),
    m_filePath(filePath), m_context(context)
{}

DBGProfileData::~DBGProfileData()
{}


void DBGProfileData::setFilePath(const QString& filePath)
{
  m_filePath = filePath;
}

void DBGProfileData::setContext(const QString& context)
{
  m_context = context;
}
void DBGProfileData::setLine(int line)
{
  m_line = line;
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

const QString& DBGProfileData::filePath() const
{
  return m_filePath;
}

const QString& DBGProfileData::context() const
{
  return m_context;
}

int DBGProfileData::line() const
{
  return m_line;
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

//-------------------------------------------------------------------------

DBGProfileDialog::DBGProfileDialog(QWidget* parent, const char* name)
    : KDialog(parent, name),
    m_listView(0), m_profileData(0)
{
  setCaption("Profile");
  
  QVBoxLayout* l = new QVBoxLayout(this);

  m_listView = new KListView(this);
  m_listView->setSorting(-1);
  m_listView->setAllColumnsShowFocus(true);

  m_listView->addColumn("File path");
  m_listView->addColumn("Line");
//   m_listView->addColumn("Context");
  m_listView->addColumn("Hits");
  m_listView->addColumn("Avarage time/hits (ms)");
  m_listView->addColumn("Total time (ms)");
  m_listView->addColumn("Min time (ms)");
  m_listView->addColumn("Max time (ms)");
      
  l->addWidget(m_listView);
  
  resize(600, 200);
}

DBGProfileDialog::~DBGProfileDialog()
{
  delete m_profileData;
}

void DBGProfileDialog::clear()
{
  m_listView->clear();
}

void DBGProfileDialog::setData(DBGProfileData* data)
{
  if(m_profileData ) {
    delete m_profileData ;
  }
  m_profileData = data;

  reloadData();
}

void DBGProfileDialog::reloadData()
{
  KListViewItem* item= new KListViewItem(m_listView);
  item->setText(FilePathCol, m_profileData->filePath());
  item->setText(LineCol, QString::number(m_profileData->line()));
//   item->setText(ContextCol, m_profileData->context());
  item->setText(HitsCol, QString::number(m_profileData->hitCount()));
  item->setText(AvgCol, QString("%1").arg(m_profileData->timeAvgHit(), 0, 'f'));
  item->setText(TotalCol, QString("%1").arg(m_profileData->totalTime(), 0, 'f'));
  item->setText(MinCol, QString("%1").arg(m_profileData->minTime(), 0, 'f'));
  item->setText(MaxCol, QString("%1").arg(m_profileData->maxTime(), 0, 'f'));

  //add item to the bottom
  item->moveItem(m_listView->lastItem());
}

#include "dbgprofile.moc"
