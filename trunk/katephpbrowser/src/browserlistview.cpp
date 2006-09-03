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

#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kinputdialog.h>
#include <kmessagebox.h>

#include "browsernode.h"


BrowserListView::BrowserListView(QWidget* parent, const char* name)
  : KListView(parent, name)
{
  setAllColumnsShowFocus(true);
  setRootIsDecorated(true);
  setSorting(-1);

  setDragEnabled(true);
  setAcceptDrops (true);
//   setDropHighlighter(true);

  addColumn("");

  header()->hide();

  connect(this, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)),
          this, SLOT(slotDoubleClick(QListViewItem *, const QPoint &, int)));

  connect(this, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint& , int)),
          this, SLOT(slotContextMenuRequested(QListViewItem *, const QPoint &, int)));

  connect(this, SIGNAL(expanded(QListViewItem*)),
          this, SLOT(slotItemExpanded(QListViewItem*)));

  connect(this, SIGNAL(collapsed(QListViewItem*)),
          this, SLOT(slotItemCollapsed(QListViewItem*)));  
}

BrowserListView::~BrowserListView()
{
}

void BrowserListView::slotDoubleClick(QListViewItem *item, const QPoint&, int)
{
  if(!item) return;

  BrowserListViewItem *vitem = dynamic_cast<BrowserListViewItem*>(item);

  if(!vitem->isFolder())
  {
    emit gotoFileLine(vitem->fileURL(), vitem->line());
  }
}


void BrowserListView::slotContextMenuRequested(QListViewItem* item, const QPoint& p, int)
{

  enum { CreateFolder, DeleteFolder, RenameFolder };  
  
  KPopupMenu* menu = new KPopupMenu(this);
  menu->insertItem(i18n("Create Folder"), CreateFolder);

  BrowserListViewItem *vitem = dynamic_cast<BrowserListViewItem*>(item);

  if(vitem && vitem->isFolder())
  {
    menu->insertItem(i18n("Delete Folder"), DeleteFolder);
    menu->insertItem(i18n("Rename Folder"), RenameFolder);
  }

  int selection = menu->exec(p);
    
  QString folderName;
  bool ok = false;
  switch(selection)
  {
    case CreateFolder:
      folderName = KInputDialog::getText(i18n("New Folder"), i18n("Folder name:"), 
                      QString::null, &ok);
      if(ok)
      {
        createFolder(folderName);
      }
      break;
    case DeleteFolder:
        deleteFolder(item);
      break;
    case RenameFolder:
      folderName = KInputDialog::getText(i18n("New Folder"), i18n("Folder name:"), 
                      QString::null, &ok);
      if(folderExists(folderName))
      {
        KMessageBox::sorry(0, i18n("Folder \"") + folderName + i18n("\" already exists"));
      }
      else
      {
        item->setText(0, folderName);
      }
      break;      
  }

  delete menu;      
}

void BrowserListView::slotItemExpanded(QListViewItem* item_)
{
  BrowserListViewItem* item = dynamic_cast<BrowserListViewItem*>(item_);
  QString path = item->stringPath();
  if(m_expanded.findIndex(path) == -1)
  {
    m_expanded.push_back(path);
  }
}

void BrowserListView::slotItemCollapsed(QListViewItem* item_)
{
  BrowserListViewItem* item = dynamic_cast<BrowserListViewItem*>(item_);
  m_expanded.remove(item->stringPath());
}

void BrowserListView::reexpandItems()
{
  BrowserListViewItem* item;
  QValueList<QString>::iterator it;

  QValueList<QString> paths = m_expanded;

  for(it = paths.begin(); it != paths.end(); it++)
  {
    QString str = (*it);
    item = getItemFromPath(str);
    if(item)
    {
      item->setOpen(true);
    }
  }
}

BrowserListViewItem* BrowserListView::getItemFromPath(QString path)
{
  int sections = path.contains('/') + 1;
  int index = 0;
  QString s, comp;
  BrowserListViewItem* item = dynamic_cast<BrowserListViewItem*>(firstChild());

  do
  {
    if((!item) || (index >= sections)) break;
    s = item->text(0);
    if(item->text(0) == path.section('/', index, index))
    {
      index++;
      if(index == sections)
      {
        return dynamic_cast<BrowserListViewItem*>(item);
      }
      else
      {
        item = dynamic_cast<BrowserListViewItem*>(item->firstChild());
      }
    }
    else
    {
      item = dynamic_cast<BrowserListViewItem*>(item->nextSibling());
      continue;
    }
  }
  while(true);

  return NULL;
}

void BrowserListView::createFolder(const QString& folderName)
{
  BrowserListViewItem* nitem = 0;
  if(folderExists(folderName))
  {
    KMessageBox::sorry(0, i18n("Folder \"") + folderName + i18n("\" already exists"));    
  }
  else
  {    
    nitem = new BrowserListViewItem(this, folderName);

    m_folders[folderName] = nitem;

    QListViewItem* after = lastRootItem();
    if(after)
    {
      nitem->moveItem(after);
    }
  }
}

bool BrowserListView::folderExists(const QString& folderName)
{
  BrowserListViewItem* item =
    dynamic_cast<BrowserListViewItem*>(firstChild());

  while(item)
  {
    if(item->isFolder() && (item->text(0) == folderName))
    {
      return true;
    }
    item =
      dynamic_cast<BrowserListViewItem*>(item->nextSibling());
  }
  return false;
}

void BrowserListView::deleteFolder(QListViewItem* folderItem)
{
  
  QListViewItem* child = folderItem->firstChild();
  QPtrList<QListViewItem> list;
  while(child)
  {
    list.append(child);
    child = child->nextSibling();
  }
 
  QListViewItem* previous = itemAtIndex(itemIndex(folderItem)-1);
  //get the root of the above item
  while(previous && previous->parent())
  {
    previous = previous->parent();
  }

  //move everyone to the root level
  for(child = list.first(); child; child = list.next())
  {
    folderItem->takeItem(child);
    insertItem(child);
    //move the item after the folder's previous
    child->moveItem(previous);
    previous = child;
  }

  takeItem(folderItem);
}

void BrowserListView::movableDropEvent(QListViewItem* parent_, QListViewItem* afterme_)
{
  BrowserListViewItem* curr = dynamic_cast<BrowserListViewItem*>(currentItem());
  BrowserListViewItem* parent  = dynamic_cast<BrowserListViewItem*>(parent_);
  BrowserListViewItem* curparent  = dynamic_cast<BrowserListViewItem*>(currentItem()->parent());
  if((!parent || parent->isFolder()) && (!curparent || curparent->isFolder()))
  {
    //prevent moving folders to a parent
    if(curr->isFolder() && parent)
    {
      return;
    }

    BrowserListViewItem* afterme  = dynamic_cast<BrowserListViewItem*>(afterme_);

    //prevent moving top leve items
    if(!curparent && !parent && (!afterme || !afterme->isFolder()))
    {
      return;
    }

    //if afterme is folder, lets go inside it
    if(afterme && afterme->isFolder())
    {
      parent = afterme;
      afterme = 0;
    }

    KListView::movableDropEvent(parent, afterme);
  }
}


void BrowserListView::addFileNode(BrowserNode* root)
{
  BrowserListViewItem* folder = 0;
  BrowserListViewItem* item   = 0;

  if(m_map.contains(root->fileURL()))
  {

    item = m_map[root->fileURL()].first;    
    item->clear();

    //get the folder where the item is (if any)
    folder = dynamic_cast<BrowserListViewItem*>(item->parent());

    //remove the old list
    QPtrList<BrowserNode> oldlist = m_map[root->fileURL()].second;
    oldlist.setAutoDelete(true);
    oldlist.clear();    
  }

  m_map[root->fileURL()].second = root->childs();

  if(!item)
  {
    if(folder)
    {
      item = new BrowserListViewItem(folder, root->fileURL());
    }
    else
    {
      item = new BrowserListViewItem(this, root->fileURL());
    }
    m_map[root->fileURL()].first = item;
  }

  //put in the right folder
  for(QMap<QString, QStringList>::iterator it = m_folderStructure.begin(); it != m_folderStructure.end(); ++it)
  {
    for(QStringList::iterator lit = it.data().begin(); lit != it.data().end(); ++lit)
    {
      if(item->text(0) == *lit)
      {
        moveItem(item, m_folders[it.key()],0);
      }
    }
  }

  QPtrList<BrowserNode> list = root->childs();

  if(root->hasError())
  {
    item->hasError(true);
  }
  else
  {
    item->hasError(false);
    for(BrowserNode* node = list.last(); node; node = list.prev())
    {
      addNode(node, item);
    }
  }

  reexpandItems();
}

void BrowserListView::addNode(BrowserNode* node, BrowserListViewItem* parent)
{
  BrowserListViewItem* item;
  if (parent)
  {
    item = new BrowserListViewItem(parent, node);
  }
  else
  {
    item = new BrowserListViewItem(this, node);
  }  

  QString name = node->name();
  item->setText(0, name);

  QPtrList<BrowserNode> list = node->childs();

  if(list.count() > 0)
  {
    for(BrowserNode* node = list.last(); node; node = list.prev())
    {
      addNode(node, item);
    }
  }
}

BrowserListViewItem* BrowserListView::lastRootItem()
{
  QListViewItem* item = lastItem();
  while(item && item->parent()) 
  {
    item = item->parent();
  }
  return dynamic_cast<BrowserListViewItem*>(item);  
}

QMap<QString, QStringList> BrowserListView::folderStructure()
{
  QMap<QString, QStringList> map;

  QString s;
  BrowserListViewItem* child = dynamic_cast<BrowserListViewItem*>(firstChild());
  while(child)
  {
    if (child->isFolder())
    {
      map[child->text(0)] = child->folderChilds();
    }
    child = dynamic_cast<BrowserListViewItem*>(child->nextSibling());
  }
  return map;
}

void BrowserListView::setFolderStructure(const QMap<QString, QStringList>& map)
{
  m_folderStructure = map;

  QMap<QString, QStringList>::const_iterator it;
  for(it = map.begin(); it != map.end(); ++it)
  {
    createFolder(it.key());
  }
}

void BrowserListView::clear()
{
  QMap<KURL, QPair<BrowserListViewItem*, QPtrList<BrowserNode> > >::iterator it;
  for(it = m_map.begin(); it != m_map.end(); ++it ) {

    QPtrList<BrowserNode> oldlist = it.data().second;
    oldlist.setAutoDelete(true);
    oldlist.clear();
  }

  m_map.clear();
  m_folders.clear();
  m_folderStructure.clear();

  KListView::clear();
}

// QListViewItem* BrowserListView::findItem(const QString& name)
// {
//   QListViewItem* item = firstChild();
//   while(item)
//   {
//     if(item->text(0) == name)
//     {
//       return item;
//     }
//     item = item->nextSibling();
//   }
//   return 0;
// }

#include "browserlistview.moc"
