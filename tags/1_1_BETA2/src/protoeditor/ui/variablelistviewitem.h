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

#ifndef VARIABLESLISTVIEWITEM_H
#define VARIABLESLISTVIEWITEM_H

#include <klistview.h>

class Variable;

class VariableListViewItem : public KListViewItem {
public:
  VariableListViewItem(KListView *parent);
  VariableListViewItem(KListViewItem *parent);

  VariableListViewItem(KListView *parent, Variable* variable);
  VariableListViewItem(KListViewItem *parent, Variable* variable);

  virtual ~VariableListViewItem();

  void setVariable(Variable* variable);
  void clearChilds();

  virtual void insertItem(VariableListViewItem *item);

  Variable* variable();

  virtual QListViewItem* lastItem();

  virtual void setRenameEnabled( int col, bool b );

  void showVariable();

  QString stringPath();

private:  
  void deleteVar();

  Variable* m_variable;
  bool m_isRenameable;
  bool m_isRoot;
};

#endif
