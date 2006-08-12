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

#include "browserlistviewitem.h"
#include <kiconloader.h>

#include "browsernode.h"

BrowserListViewItem::BrowserListViewItem(KListView *parent, const KURL& url)
  : KListViewItem(parent), m_line(0), m_url(url)
{
  setPixmap(0, SmallIcon("source_php"));
  setText(0, url.filename());
}

BrowserListViewItem::BrowserListViewItem(KListView *parent, BrowserNode* node)
  : KListViewItem(parent), m_line(node->line()), m_url(node->fileURL())
{
  setText(0, node->name());
  loadPixmap(node);
}

BrowserListViewItem::BrowserListViewItem(KListViewItem *parent, BrowserNode* node)
  : KListViewItem(parent), m_line(node->line()), m_url(node->fileURL())
{
  setText(0, node->name());
  loadPixmap(node);
}

BrowserListViewItem::~BrowserListViewItem()
{
}

KURL BrowserListViewItem::getFileURL()
{
  return m_url;
}

int BrowserListViewItem::getLine()
{
  return m_line;
}

void BrowserListViewItem::loadPixmap(BrowserNode* node)
{
  switch(node->type())
  {
    case BrowserNode::ClassType:
      setPixmap(0, SmallIcon("kcmdf"));
      break;
    case BrowserNode::InterfaceType:
      setPixmap(0, SmallIcon("gear"));
      break;
    case BrowserNode::ConstType:
      setPixmap(0, SmallIcon("music_cross"));
      break;
    case BrowserNode::AttributeType:
      //check for access mode
      setPixmap(0, SmallIcon("math_brace"));
      break;
    case BrowserNode::MethodType:
      //check for access mode
      setPixmap(0, SmallIcon("next"));
      break;
  }
}
