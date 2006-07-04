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

#include "watchlistview.h"
#include "variablelistviewitem.h"
#include <kpopupmenu.h>

#include <kapplication.h>
#include <qclipboard.h>
#include <klocale.h>


WatchListView::WatchListView(int id, QWidget *parent, const char *name)
 : VariableListView(id, parent, name)
{

}


WatchListView::~WatchListView()
{
}

void WatchListView::slotContextMenuRequested(QListViewItem* item, const QPoint& p, int)
{
  enum { CopyVarItem, CopyValueItem, DeleteWatch, DeleteAllWatches };
  
  KPopupMenu* menu = new KPopupMenu(this);
  menu->insertItem(i18n("Copy variable"), CopyVarItem);
  menu->insertItem(i18n("Copy value"), CopyValueItem);
  menu->insertItem(i18n("Delete"), DeleteWatch);
  menu->insertItem(i18n("Delete all"), DeleteAllWatches);
 
  if(!item)
  {
    menu->setItemEnabled(CopyVarItem, false);
    menu->setItemEnabled(CopyValueItem, false);
    menu->setItemEnabled(DeleteWatch, false);
  }

  if(childCount() == 0) 
  {
    menu->setItemEnabled(DeleteAllWatches, false);
  }

  int selection = menu->exec(p);
  if(selection == -1)
  {
    delete menu;
    return;
  }
  
  QClipboard* clip = kapp->clipboard();

  VariableListViewItem* vitem =
      dynamic_cast<VariableListViewItem*>(item);
  
  switch(selection)
  {
    case CopyVarItem:
      clip->setText(vitem->variable()->toString(), QClipboard::Clipboard);
      break;
    case CopyValueItem:
      clip->setText(vitem->variable()->value()->toString(), QClipboard::Clipboard);
      break;
    case DeleteWatch:
      removeWatch(vitem);
      break;
    case DeleteAllWatches:
      removeAllWatches();
  }

  delete menu;   
}

void WatchListView::addWatch(Variable* var) {
  VariableListViewItem* item = dynamic_cast<VariableListViewItem*>(
      findItem(var->name(), VariableListView::NameCol));

  if(item) {
    bool opened = item->isOpen();
    item->clearChilds();
    item->setVariable(var);
    if(!var->value()->isScalar())
    {
      item->setOpen(opened);
    }
  }
  else
  {
    addVariable(var);
  }
}

void WatchListView::addWatch(const QString& expression)
{
  Variable* var = new Variable(expression);
  VariableScalarValue* value = new VariableScalarValue(var);
  var->setValue(value);
  addWatch(var); 
}

QStringList WatchListView::watches()
{
  QStringList list;
  VariableListViewItem* item = dynamic_cast<VariableListViewItem*>(firstChild());

  while(item)
  {
    list << item->variable()->name();
    item = dynamic_cast<VariableListViewItem*>(item->nextSibling());
  }
  return list;
}

void WatchListView::reset() 
{
  QStringList list = watches();
  clear();

  QStringList::iterator it;
  for(it = list.begin(); it != list.end(); it++) 
  {
    addWatch(*it);
  }
}

void WatchListView::keyPressEvent(QKeyEvent* e) {
  if(e->key() == Qt::Key_Delete) {

    VariableListViewItem* item =
      dynamic_cast<VariableListViewItem*>(currentItem());

    removeWatch(item);
  } 
  else 
  {
    VariableListView::keyPressEvent(e);
  }
}

void WatchListView::removeWatch(VariableListViewItem* item)
{
  if(item) {
    emit sigWatchRemoved(item->variable());
    delete item;
  }
}

void WatchListView::removeAllWatches()
{

  VariableListViewItem* item = dynamic_cast<VariableListViewItem*>(firstChild());
  VariableListViewItem* other;
  while(item)
  {
    other = item;
    item = dynamic_cast<VariableListViewItem*>(item->nextSibling());
    removeWatch(other);
  } 
}

#include "watchlistview.moc"
