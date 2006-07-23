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

#include "variablelistview.h"
#include "variablelistviewitem.h"

#include <klocale.h>
#include <kdebug.h>
#include <kpopupmenu.h>
#include <kapplication.h>

#include <qheader.h>
#include <qclipboard.h>


VariableListView::VariableListView(int id, QWidget *parent, const char *name)
    : KListView(parent, name), m_id(id), m_variables(0), m_isReadOnly(false)
{
  setAllColumnsShowFocus(true);
  setRootIsDecorated(true);
  setSorting(-1);

  addColumn(i18n("Name"));
  addColumn(i18n("Value"));
  addColumn(i18n("Type"));

  setColumnWidthMode(VariableListView::NameCol,  Manual);
  setColumnWidthMode(VariableListView::ValueCol, Manual);
  setColumnWidthMode(VariableListView::TypeCol,  Manual);

  setColumnWidth(VariableListView::NameCol,  150);
  setColumnWidth(VariableListView::ValueCol, 150);
  setColumnWidth(VariableListView::TypeCol,  150);

  connect(this, SIGNAL(expanded(QListViewItem*)),
          this, SLOT(slotItemExpanded(QListViewItem*)));
  connect(this, SIGNAL(collapsed(QListViewItem*)),
          this, SLOT(slotItemCollapsed(QListViewItem*)));

  connect(this, SIGNAL(itemRenamed(QListViewItem*, int, const QString&)),
          this, SLOT(slotItemRenamed(QListViewItem*, int, const QString&)));

  connect(this, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)),
          this, SLOT(slotDoubleClick(QListViewItem *, const QPoint &, int)));

  //connect(m_menu, SIGNAL(activated(int)), this, SLOT(slotCopyVarToClipboard(int)));

  connect(this, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint& , int)),
          this, SLOT(slotContextMenuRequested(QListViewItem *, const QPoint &, int)));

  m_variables = new VariableList_t;
}

int VariableListView::id()
{
  return m_id;
}

void VariableListView::setReadOnly(bool readOnly)
{
  m_isReadOnly = readOnly;

  QListViewItem* item = firstChild();
  while(item)
  {
    item->setRenameEnabled(VariableListView::ValueCol, !m_isReadOnly);
    item = item->nextSibling();
  }
}

bool VariableListView::isReadOnly()
{
  return m_isReadOnly;
}


/*
 * The ListView doesn't start renaming if the item is not previously selected
 * And that sux...so, here we go.
 */
void VariableListView::slotDoubleClick( QListViewItem *item, const QPoint &, int col)
{
  if(col == VariableListView::ValueCol)
  {
    item->startRename(VariableListView::ValueCol);
  }
}

VariableListView::~VariableListView()
{
  delete m_variables;
}

void VariableListView::slotItemRenamed(QListViewItem * item, int, const QString & text)
{

  VariableListViewItem* vitem =
    dynamic_cast<VariableListViewItem*>(item);

  //VariableScalarValue* value = new VariableScalarValue();

  dynamic_cast<VariableScalarValue*>(
    vitem->variable()->value())->set(text);

  emit sigVarModified(vitem->variable());
}

void VariableListView::slotItemCollapsed(QListViewItem* item)
{
  markColapsed(dynamic_cast<VariableListViewItem*>(item));
}

void VariableListView::slotItemExpanded(QListViewItem* item)
{
  VariableListViewItem* converted = dynamic_cast<VariableListViewItem*>(item);  
  populateChildren(converted);
  markExpanded(converted);
}

void VariableListView::slotContextMenuRequested(QListViewItem* item, const QPoint& p, int)
{
  enum { CopyVarItem, CopyValueItem };
  
  KPopupMenu* menu = new KPopupMenu(this);
  menu->insertItem(i18n("Copy variable"), CopyVarItem);
  menu->insertItem(i18n("Copy value"), CopyValueItem);
 
  if(!item)
  {
    menu->setItemEnabled(CopyVarItem, false);
    menu->setItemEnabled(CopyValueItem, false);
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
  }

  delete menu;      
}

void VariableListView::markColapsed(VariableListViewItem* item)
{
  m_expanded.remove(item->stringPath());  
}

void VariableListView::markExpanded(VariableListViewItem* item)
{
  m_expanded.push_back(item->stringPath());
}

void VariableListView::populateChildren(VariableListViewItem* item)
{
  /*
    Populate array/object children at request time
    so it can support recursive references easily
  */

  if((item->childCount() > 0) || (item->variable()->value()->isScalar()))
  {
    return;
  }

  VariableListValue* v = dynamic_cast<VariableListValue*>((item)->variable()->value());
  if(!v->initialized())
  {
    m_needChildList.append(item);
    emit sigNeedChildren(id(), item->variable());
  }
  else
  {
    VariableList_t* list = v->list();
    addVariables(list, item);
  }  
}

void VariableListView::setVariables(VariableList_t* vars)
{
  /* saves the position of the viewport
   * ('cause when the view is repopulated, the scroll goes up)
   */
  int contentX = contentsX();
  int contentY = contentsY();

  /* backup the current item selected
   */
  QString currentSelected;
  VariableListViewItem* item;

  item = dynamic_cast<VariableListViewItem*>(selectedItem());
  if(item)
  {
    currentSelected = item->stringPath();
  }

  clear();

  /* finally, add the new ones
   */
  addVariables(vars);

  /* expand the new items according to the previous state
   */
  reexpandItems();

  /* resets the viewport position
   */
  setContentsPos(contentX, contentY);

  delete vars;
}

void VariableListView::updateVariable(Variable*)
{
  VariableListViewItem* item = m_needChildList.first();
  m_needChildList.remove();
  item->setOpen(false);
  item->setOpen(true);
  reexpandItems();
}

void VariableListView::addVariables(VariableList_t* vars, VariableListViewItem* parent)
{
  for(VariableList_t::iterator it = vars->begin(); it != vars->end(); ++it)
  {
    addVariable(*it, parent);
  }
}

void VariableListView::reexpandItems()
{
  VariableListViewItem* item;
  QValueList<QString>::iterator it;

  QValueList<QString> paths = m_expanded;

  loop:

  for(it = paths.begin(); it != paths.end(); it++)
  {
    if(itemStatus(*it) != 1) // != needchild (exists or is invalid)
    {
      item = getItemFromPath(*it);
      //check for existence: variable might not exist in the scope anymore
      //check for scalar: variable (name) might have changed from list to scalar
      if(item && !item->variable()->value()->isScalar())
      {
        item->setOpen(true);
      }
      else
      {
        m_expanded.remove(*it);
      }
      paths.remove(*it);
      break;
    }
    else
    {
      return;
    }
  }

  if(paths.count() == 0)
  {
    return;
  } 
  else
  {
    goto loop;
  }
}

VariableListViewItem* VariableListView::getItemFromPath(QString path)
{
  int sections = path.contains('/') + 1;
  int index = 0;
  QString s, comp;
  VariableListViewItem* item = dynamic_cast<VariableListViewItem*>(firstChild());

  do
  {
    if((!item) || (index >= sections)) break;
    s = item->text(VariableListView::NameCol);
    if(item->text(VariableListView::NameCol) == path.section('/', index, index))
    {
      index++;
      if(index == sections)
      {
        return dynamic_cast<VariableListViewItem*>(item);
      }
      else
      {
        populateChildren(item);
        item = dynamic_cast<VariableListViewItem*>(item->firstChild());
      }
    }
    else
    {
      item = dynamic_cast<VariableListViewItem*>(item->nextSibling());
      continue;
    }
  }
  while(true);

  return NULL;
}

int VariableListView::itemStatus(QString path)
{
  int sections = path.contains('/') + 1;
  int index = 0;
  QString s, comp;
  VariableListViewItem* item = dynamic_cast<VariableListViewItem*>(firstChild());

  //exists: 0, needc = 1, invalid = 2
  do
  {
    if((!item) || (index >= sections)) break;
    s = item->text(VariableListView::NameCol);
    if(item->text(VariableListView::NameCol) == path.section('/', index, index))
    {
      index++;
      if(index == sections)
      {
        return 0;
      }
      else
      {
        if((item->childCount() > 0) || (item->variable()->value()->isScalar()))
        {
          return 0;
        }

        VariableListValue* v = dynamic_cast<VariableListValue*>((item)->variable()->value());
        if(!v->initialized())
        {
          return 1;
        }
        else
        {
          return 0;
        }
      }
    }
    else
    {
      item = dynamic_cast<VariableListViewItem*>(item->nextSibling());
      continue;
    }
  }
  while(true);

  return 2;  
}

void VariableListView::addVariable(Variable* variable, VariableListViewItem* parent)
{
  VariableListViewItem* item;

  if(!parent)
  {
    item = new VariableListViewItem(this, variable);
    //add new item to the bottom of the list
    item->moveItem(lastRootItem());
  }
  else
  {
    item = new VariableListViewItem(parent, variable);
    item->moveItem(item->lastItem());
  }

  m_variables->append(variable);
}

VariableListViewItem* VariableListView::lastRootItem() {
  QListViewItem* item = lastItem();
  while(item->parent()) {
    item = item->parent();
  }
  return dynamic_cast<VariableListViewItem*>(item);
}

#include "variablelistview.moc"
