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
#ifndef BROWSERLISTVIEWITEM_H
#define BROWSERLISTVIEWITEM_H

#include <klistview.h>
#include <kurl.h>

class BrowserNode;

class BrowserListViewItem : public KListViewItem
{
public:
  BrowserListViewItem(KListView*, const QString&);

  BrowserListViewItem(KListView*, const KURL&);

  BrowserListViewItem(KListViewItem*, const KURL&);

  BrowserListViewItem(KListView*, BrowserNode*);
//   BrowserListViewItem(KListViewItem *parent);
  BrowserListViewItem(KListViewItem*, BrowserNode*);

  ~BrowserListViewItem();

  bool isFolder();

  KURL fileURL();
  int line();

  QString stringPath();

  QStringList folderChilds();

  BrowserListViewItem* lastItem();

  void clear();
private:
  void load(BrowserNode*);
  bool isClassLevel();

  int m_line;
  KURL m_url;
  bool m_isFolder;
  bool m_isClassLevel;
};

#endif
