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

class BrowserListViewItem : public KListViewItem
{
public:
  BrowserListViewItem(KListView *parent);
  BrowserListViewItem(KListView *parent, const QString&, const KURL&, int);
  BrowserListViewItem(KListViewItem *parent);
  BrowserListViewItem(KListViewItem *parent, const QString&, const KURL&, int);

  ~BrowserListViewItem();

  KURL getFileURL();
  int getLine();
private:

  int m_line;
  KURL m_url;
};

#endif
