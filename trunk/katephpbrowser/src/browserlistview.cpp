/***************************************************************************
 *   Copyright (C) 2006 by Thiago Silva                                    *
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
#include "browserlistview.h"
#include <klocale.h>
#include <qheader.h>

#include "browsernode.h"

BrowserListView::BrowserListView(QWidget* parent, const char* name)
  : KListView(parent, name)
{
  setAllColumnsShowFocus(true);
  setRootIsDecorated(true);
  setSorting(-1);

  addColumn("");

  header()->hide();
}

BrowserListView::~BrowserListView()
{
}

void BrowserListView::loadNodes(QValueList<BrowserNode*> list)
{
  m_list = list;
  for(QValueList<BrowserNode*>::iterator it = list.begin(); it != list.end(); ++it)
  {
    addNode(*it);
  }
}

void BrowserListView::addNode(BrowserNode* node, QListViewItem* parent)
{
  QListViewItem* item;
  if (parent)
  {
    item = new QListViewItem(parent);
  }
  else
  {
    item = new QListViewItem(this);
  }
  item->setText(0, node->name());

  if(node->childs().count() > 0)
  {
    QValueList<BrowserNode*> list = node->childs();
    for(QValueList<BrowserNode*>::iterator it = list.begin(); it != list.end(); ++it)
    {
      addNode(*it, item);
    }
  }
}

#include "browserlistview.moc"
