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
 
#ifndef DBGPROFILE_H
#define DBGPROFILE_H

#include <kdialog.h>

class DBGProfileData
{
public:
  DBGProfileData();
  DBGProfileData(const QString&, const QString&, int, int, double, double, double, double);
  ~DBGProfileData();

  void setFilePath(const QString&);
  void setContext(const QString&);
  void setLine(int);
  void setHitCount(int);

  void setTimeAvgHit(double);
  void setTotalTime(double);
  void setMinTime(double);
  void setMaxTime(double);

  const QString& filePath() const;
  const QString& context() const;
  int line() const;
  int hitCount() const;
  double timeAvgHit() const;
  double totalTime() const;
  double minTime() const;
  double maxTime() const;
private:
  int m_line;
  int m_hitCount;
  double m_timeAvgHit;
  double m_totalTime;
  double m_minTime;
  double m_maxTime;

  QString m_filePath;
  QString m_context;
};

//---------------------------------------------------------------

class KListView;

class DBGProfileDialog : public KDialog
{
  Q_OBJECT
public:
  DBGProfileDialog(QWidget* parent = 0, const char* name = 0);
  ~DBGProfileDialog();

  void clear();
  void setData(DBGProfileData*);
private:
  enum {FilePathCol, LineCol, /*ContextCol,*/ HitsCol, AvgCol, TotalCol, MinCol, MaxCol};

  void reloadData();
  
  KListView* m_listView;
  DBGProfileData* m_profileData;
};

#endif
