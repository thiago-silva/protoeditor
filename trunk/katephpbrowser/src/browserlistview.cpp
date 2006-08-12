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
#include "browserlistviewitem.h"

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

  connect(this, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)),
          this, SLOT(slotDoubleClick(QListViewItem *, const QPoint &, int)));

}

BrowserListView::~BrowserListView()
{
}

void BrowserListView::slotDoubleClick(QListViewItem *item, const QPoint&, int)
{
  if(!item) return;

  BrowserListViewItem *vitem = dynamic_cast<BrowserListViewItem*>(item);

  emit gotoFileLine(vitem->getFileURL(), vitem->getLine());
}

void BrowserListView::loadFileNodes(const KURL& url, const QValueList<BrowserNode*>& list)
{
  BrowserListViewItem* item = new BrowserListViewItem(this, url.filename(), url, 0);

  for(QValueList<BrowserNode*>::const_iterator it = list.begin(); it != list.end(); ++it)
  {
    addNode(*it, item);
  }
}

void BrowserListView::addNode(BrowserNode* node, BrowserListViewItem* parent)
{
  BrowserListViewItem* item;
  if (parent)
  {
    item = new BrowserListViewItem(parent, node->name(), node->fileURL(), node->line());
  }
  else
  {
    item = new BrowserListViewItem(this, node->name(), node->fileURL(), node->line());
  }
  QString name = node->name();
  item->setText(0, name);

  QValueList<BrowserNode*> list = node->childs();

  if(list.count() > 0)
  {    
    for(QValueList<BrowserNode*>::iterator it = list.begin(); it != list.end(); ++it)
    {
      addNode(*it, item);
    }
  }
}

#include "browserlistview.moc"
