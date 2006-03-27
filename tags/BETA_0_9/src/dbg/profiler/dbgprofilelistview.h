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

#ifndef DBGProfileListView_H
#define DBGProfileListView_H

#include <klistview.h>

class DBGProfileData;

class DBGProfileListViewItem : public KListViewItem
{
public:
  DBGProfileListViewItem(KListView *parent, DBGProfileData* data, int view);
  DBGProfileListViewItem(KListViewItem *parent, DBGProfileData* data, int view);
  ~DBGProfileListViewItem();

  DBGProfileListViewItem* lastItem();

  DBGProfileData* data();

private:
  bool isRoot();
  void loadData(int view);
  void calculateTotalTime();

  DBGProfileData* m_data;
};

//------------------------------------------------------------------------------

class DBGProfileListView : public KListView
{
  Q_OBJECT
public:
  enum { ModuleView, ContextView, DetailedView };

  enum {LocationCol, LineCol, HitsCol, AvgCol, TotalCol, MinCol, MaxCol, ChartCol};

  DBGProfileListView(QWidget *parent, const char *name = 0);
  ~DBGProfileListView();

  void setView(int);
  int view();

  void addData(DBGProfileData*);

signals:
    void sigDoubleClick(const QString& filePath, int line);
    
private slots:
  void slotDoubleClick(QListViewItem *, const QPoint &, int );
  
private:
  DBGProfileListViewItem* getRootItem(DBGProfileData* data);
  DBGProfileListViewItem* getModuleRootItem(int modid);
  DBGProfileListViewItem* getContextRootItem(int ctxid);
  QListViewItem* lastRootItem();

  void addToList(DBGProfileData* data);
  void reloadList();

  int m_view;
  QValueList<DBGProfileData*> m_dataList;
};

#endif
