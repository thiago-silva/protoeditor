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

#include "dbgprofilelistview.h"
#include "dbgprofiledata.h"
#include "dbgprofilelocation.h"

DBGProfileListViewItem::DBGProfileListViewItem(KListView* parent, DBGProfileData* data, int view)
  : KListViewItem(parent), m_data(data)
{
  loadData(view);
}

DBGProfileListViewItem::DBGProfileListViewItem(KListViewItem *parent, DBGProfileData* data, int view)
  : KListViewItem(parent), m_data(data)
{
  loadData(view);
}

DBGProfileListViewItem::~DBGProfileListViewItem()
{
  delete m_data;
}

bool DBGProfileListViewItem::isRoot()
{
  return parent() == 0;
}

void DBGProfileListViewItem::loadData(int view)
{
  if(isRoot())
  {
    switch(view)
    {
      case DBGProfileListView::ModuleView:
      case DBGProfileListView::ContextView:
        setText(DBGProfileListView::LocationCol, m_data->location()->moduleName().section('/',-1));
        break;
      case DBGProfileListView::DetailedView:
        setText(DBGProfileListView::LocationCol, m_data->location()->moduleName().section('/',-1));
        setText(DBGProfileListView::LineCol, QString::number(m_data->location()->line()));
        setText(DBGProfileListView::HitsCol, QString::number(m_data->hitCount()));
        setText(DBGProfileListView::AvgCol, QString().sprintf("%.3f", m_data->timeAvgHit()));
        setText(DBGProfileListView::TotalCol, QString().sprintf("%.3f", m_data->totalTime()));
        setText(DBGProfileListView::MinCol, QString().sprintf("%.3f", m_data->minTime()));
        setText(DBGProfileListView::MaxCol, QString().sprintf("%.3f", m_data->maxTime()));
        break;
    }
//     this->moveItem(lastItem());
  }
  else
  {
    setText(DBGProfileListView::LocationCol, m_data->location()->moduleName().section('/',-1));
    setText(DBGProfileListView::LineCol, QString::number(m_data->location()->line()));
    setText(DBGProfileListView::HitsCol, QString::number(m_data->hitCount()));
    setText(DBGProfileListView::AvgCol, QString().sprintf("%.3f", m_data->timeAvgHit()));
    setText(DBGProfileListView::TotalCol, QString().sprintf("%.3f", m_data->totalTime()));
    setText(DBGProfileListView::MinCol, QString().sprintf("%.3f", m_data->minTime()));
    setText(DBGProfileListView::MaxCol, QString().sprintf("%.3f", m_data->maxTime()));
    calculateTotalTime();
  }
}

void DBGProfileListViewItem::calculateTotalTime()
{
  
  QListViewItem* item = firstChild();

  double totalTime = 0;
  while(item)
  {
    totalTime += dynamic_cast<DBGProfileListViewItem*>(item)->m_data->totalTime();
    item = item->nextSibling();
  }

  setText(DBGProfileListView::TotalCol, QString().sprintf("%.3f", totalTime));
}

DBGProfileListViewItem* DBGProfileListViewItem::lastItem()
{
  QListViewItem* current = this;
  QListViewItem* last = 0;

  while((current = current->nextSibling()) != 0)
  {
    last = current;
  }
  return dynamic_cast<DBGProfileListViewItem*>(last);
}


DBGProfileData* DBGProfileListViewItem::data()
{
  return m_data;
}

    
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------




DBGProfileListView::DBGProfileListView(QWidget *parent, const char *name)
  : KListView(parent, name), m_view(DetailedView)
{
  setSorting(-1);
  setAllColumnsShowFocus(true);
  setRootIsDecorated(true);

  addColumn("Location");
  addColumn("Line");
  addColumn("Hits");
  addColumn("Avarage time (ms)");
  addColumn("Total time (ms)");
  addColumn("Min time (ms)");
  addColumn("Max time (ms)");
  //   addColumn("Chart");

  setColumnAlignment(LineCol, Qt::AlignHCenter);
  setColumnAlignment(HitsCol, Qt::AlignHCenter);
  setColumnAlignment(AvgCol, Qt::AlignHCenter);
  setColumnAlignment(TotalCol, Qt::AlignHCenter);
  setColumnAlignment(MinCol, Qt::AlignHCenter);
  setColumnAlignment(MaxCol, Qt::AlignHCenter);
}


DBGProfileListView::~DBGProfileListView()
{

}

void DBGProfileListView::setView(int view)
{
  m_view = view;
  reloadList();
}

int DBGProfileListView::view()
{
  return m_view;
}

void DBGProfileListView::addData(DBGProfileData* data)
{
  m_dataList.append(data);
  addToList(data);
}

void DBGProfileListView::addToList(DBGProfileData* data)
{
   DBGProfileListViewItem* item;
   DBGProfileListViewItem* parent;

  switch(m_view)
  {
    case ModuleView:
      parent = getModuleRootItem(data->location()->moduleId());
      break;
    case ContextView:
      parent = getContextRootItem(data->location()->contextId());
      break;
    case DetailedView:
      parent = 0;
      break;//return;
  }
      
  if(parent == 0)
  {
    item = new DBGProfileListViewItem(this, data, m_view);
    item->moveItem(lastItem());
  }
  else
  {
    item = new DBGProfileListViewItem(parent, data, m_view);
    item->moveItem(parent->lastItem());
  }
}

DBGProfileListViewItem* DBGProfileListView::getModuleRootItem(int modid)
{
  DBGProfileListViewItem* item =
      dynamic_cast<DBGProfileListViewItem*>(firstChild());
  
  while(item)
  {
    if(item->data()->location()->moduleId() == modid)
    {
      break;
    }

    item =
        dynamic_cast<DBGProfileListViewItem*>(item->nextSibling());
  }

  return item;
}

DBGProfileListViewItem* DBGProfileListView::getContextRootItem(int ctxid)
{
  DBGProfileListViewItem* item =
      dynamic_cast<DBGProfileListViewItem*>(firstChild());
  
  while(item)
  {
    if(item->data()->location()->contextId() == ctxid)
    {
      break;
    }

    item =
        dynamic_cast<DBGProfileListViewItem*>(item->nextSibling());
  }

  return item;
}

void DBGProfileListView::reloadList()
{
  clear();

  QValueList<DBGProfileData*>::iterator it;
  for(it = m_dataList.begin(); it != m_dataList.end(); ++it)
  {
    addToList(*it);
  }
}

#include "dbgprofilelistview.moc"
