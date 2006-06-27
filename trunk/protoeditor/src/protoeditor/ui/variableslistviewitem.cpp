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

#include "variableslistviewitem.h"
#include "variableslistview.h"
#include "variable.h"

#include <kiconloader.h>

VariablesListViewItem::VariablesListViewItem(KListView *parent)
  : KListViewItem(parent), m_variable(NULL), m_isRenameable(false), m_isRoot(true)
{

}

VariablesListViewItem::VariablesListViewItem(KListViewItem *parent)
  : KListViewItem(parent), m_variable(NULL), m_isRenameable(false), m_isRoot(false)
{

}

VariablesListViewItem::VariablesListViewItem(KListView *parent, Variable* variable)
  : KListViewItem(parent), m_variable(variable), m_isRenameable(false), m_isRoot(true)
{
  showVariable();
}

VariablesListViewItem::VariablesListViewItem(KListViewItem *parent, Variable* variable)
  : KListViewItem(parent), m_variable(variable),m_isRenameable(false), m_isRoot(false)
{
  //setMultiLinesEnabled(true);
  showVariable();
}

VariablesListViewItem::~VariablesListViewItem()
{  
  deleteVar();  
}

void VariablesListViewItem::deleteVar() 
{
  if(m_isRoot) //if its root, delete var (if not, the root item already deleted the var and its value list)
  {
    delete m_variable;
    m_variable = 0;
  }
}

void VariablesListViewItem::setVariable(Variable* variable)
{
  delete m_variable;
  m_variable = variable;
  showVariable();
}

void VariablesListViewItem::clearChilds()
{  
  QListViewItem* item = firstChild();
  while(item) {    
    delete item;
    item = firstChild();    
  }
  setOpen(false);
}

void VariablesListViewItem::insertItem(VariablesListViewItem *item) {
  KListViewItem::insertItem(item);
  m_variable = item->variable();
}


QString VariablesListViewItem::stringPath() {
  QListViewItem* item = this;
  QString str = item->text(VariablesListView::NameCol);
  while((item = item->parent()) != NULL) {
    str = item->text(VariablesListView::NameCol) + "/" + str;
  }
  return str;
}


Variable* VariablesListViewItem::variable() {
  return m_variable;
}

QListViewItem* VariablesListViewItem::lastItem()
{
  QListViewItem* current;
  QListViewItem* last = 0;
//   VariablesListViewItem* i;
  current = this;

  while((current = current->nextSibling()) != 0) {
//     i = dynamic_cast<VariablesListViewItem*>(current);
    last = current;
  }
  return last;
}

void VariablesListViewItem::setRenameEnabled( int col, bool b ) {
  QListViewItem::setRenameEnabled(col, b);

  QListViewItem* item = firstChild();
  while(item) {
    item->setRenameEnabled(col, b);
    item = item->nextSibling();
  }
}

void VariablesListViewItem::showVariable() {
  setText(VariablesListView::NameCol, m_variable->name());

  VariablesListView* lv = dynamic_cast<VariablesListView*>(listView());

  if(m_variable->value()->isScalar()) {
    setText(VariablesListView::ValueCol, m_variable->value()->toString());
    setRenameEnabled(VariablesListView::ValueCol, !lv->isReadOnly());
    setExpandable(false);
  } else {
    //setText(VariablesListView::Value, m_variable->value()->typeName());
    setExpandable(true);
  }

  if(m_variable->isReference()) {
    setText(VariablesListView::TypeCol, QString("&") + m_variable->value()->typeName());
  } else {
    setText(VariablesListView::TypeCol, m_variable->value()->typeName());
  }
}
