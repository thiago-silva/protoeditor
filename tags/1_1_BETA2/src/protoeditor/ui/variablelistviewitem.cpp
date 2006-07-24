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

#include "variablelistviewitem.h"
#include "variablelistview.h"
#include "variable.h"

#include <kiconloader.h>

VariableListViewItem::VariableListViewItem(KListView *parent)
  : KListViewItem(parent), m_variable(NULL), m_isRenameable(false), m_isRoot(true)
{

}

VariableListViewItem::VariableListViewItem(KListViewItem *parent)
  : KListViewItem(parent), m_variable(NULL), m_isRenameable(false), m_isRoot(false)
{

}

VariableListViewItem::VariableListViewItem(KListView *parent, Variable* variable)
  : KListViewItem(parent), m_variable(variable), m_isRenameable(false), m_isRoot(true)
{
  showVariable();
}

VariableListViewItem::VariableListViewItem(KListViewItem *parent, Variable* variable)
  : KListViewItem(parent), m_variable(variable),m_isRenameable(false), m_isRoot(false)
{
  //setMultiLinesEnabled(true);
  showVariable();
}

VariableListViewItem::~VariableListViewItem()
{  
  deleteVar();  
}

void VariableListViewItem::deleteVar() 
{
  if(m_isRoot) //if its root, delete var (if not, the root item already deleted the var and its value list)
  {
    delete m_variable;
    m_variable = 0;
  }
}

void VariableListViewItem::setVariable(Variable* variable)
{
  delete m_variable;
  m_variable = variable;
  showVariable();
}

void VariableListViewItem::clearChilds()
{  
  QListViewItem* item = firstChild();
  while(item) {    
    delete item;
    item = firstChild();    
  }
  setOpen(false);
}

void VariableListViewItem::insertItem(VariableListViewItem *item) {
  KListViewItem::insertItem(item);
  m_variable = item->variable();
}


// QString VariableListViewItem::stringPath() {
//   QListViewItem* item = this;
//   QString str = item->text(VariableListView::NameCol);
//   while((item = item->parent()) != NULL) {
//     str = item->text(VariableListView::NameCol) + "/" + str;
//   }
//   return str;
// }


Variable* VariableListViewItem::variable() {
  return m_variable;
}

QListViewItem* VariableListViewItem::lastItem()
{
  QListViewItem* current;
  QListViewItem* last = 0;
//   VariableListViewItem* i;
  current = this;

  while((current = current->nextSibling()) != 0) {
//     i = dynamic_cast<VariableListViewItem*>(current);
    last = current;
  }
  return last;
}

void VariableListViewItem::setRenameEnabled( int col, bool b ) {
  QListViewItem::setRenameEnabled(col, b);

  QListViewItem* item = firstChild();
  while(item) {
    item->setRenameEnabled(col, b);
    item = item->nextSibling();
  }
}

void VariableListViewItem::showVariable() {
  setText(VariableListView::NameCol, m_variable->name());

  VariableListView* lv = dynamic_cast<VariableListView*>(listView());

  if(m_variable->value()->isScalar()) {
    setText(VariableListView::ValueCol, m_variable->value()->toString());
    setRenameEnabled(VariableListView::ValueCol, !lv->isReadOnly());
    setExpandable(false);
  } else {
    //setText(VariableListView::Value, m_variable->value()->typeName());
    setExpandable(true);
  }

  if(m_variable->isReference()) {
    setText(VariableListView::TypeCol, QString("&") + m_variable->value()->typeName());
  } else {
    setText(VariableListView::TypeCol, m_variable->value()->typeName());
  }
}

QString VariableListViewItem::stringPath() 
{
  //this shouldn't be in Variable. The stringPath will be incorrect when
  //a deep node is actually a reference (it's parent might not be what we 
  //expect)

  QListViewItem* item = this;
  QString str = item->text(VariableListView::NameCol);
  while((item = item->parent()) != NULL) {
    str = item->text(VariableListView::NameCol) + "/" + str;
  }
  return str;
}
