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

BrowserListViewItem::BrowserListViewItem(KListView *parent, const QString& label)
  : KListViewItem(parent), m_line(0), m_url(), m_isFolder(true), m_isClassLevel(false)
{
  setText(0, label);
  setPixmap(0, SmallIcon("folder"));
}

BrowserListViewItem::BrowserListViewItem(KListView *parent, const KURL& url)
  : KListViewItem(parent), m_line(0), m_url(url), m_isFolder(false), m_isClassLevel(false)
{
  setPixmap(0, SmallIcon("source_php"));
  setText(0, url.filename());
}

BrowserListViewItem::BrowserListViewItem(KListViewItem* parent, const KURL& url)
  : KListViewItem(parent), m_line(0), m_url(url), m_isFolder(false), m_isClassLevel(false)
{
  setPixmap(0, SmallIcon("source_php"));
  setText(0, url.filename());
}

BrowserListViewItem::BrowserListViewItem(KListView *parent, BrowserNode* node)
  : KListViewItem(parent), m_line(node->line()), m_url(node->fileURL()), m_isFolder(false), m_isClassLevel(false)
{
  setText(0, node->name());
  load(node);
}

BrowserListViewItem::BrowserListViewItem(KListViewItem *parent, BrowserNode* node)
  : KListViewItem(parent), m_line(node->line()), m_url(node->fileURL()), m_isFolder(false), m_isClassLevel(false)
{
  setText(0, node->name());
  load(node);
}

BrowserListViewItem::~BrowserListViewItem()
{
}

KURL BrowserListViewItem::fileURL()
{
  return m_url;
}

int BrowserListViewItem::line()
{
  return m_line;
}

bool BrowserListViewItem::isFolder()
{
  return m_isFolder;
}

void BrowserListViewItem::load(BrowserNode* node)
{
  switch(node->type())
  {
    case BrowserNode::ClassType:
      setPixmap(0, SmallIcon("class"));
      m_isClassLevel = true;
      break;
    case BrowserNode::InterfaceType:
      setPixmap(0, SmallIcon("iface"));
      m_isClassLevel = true;
      break;
    case BrowserNode::ConstType:
      setPixmap(0, SmallIcon("const"));
      break;
    case BrowserNode::AttributeType:
      switch(node->visibility())
      {
        case BrowserNode::Public:        
          setPixmap(0, node->isStatic()?SmallIcon("svar_pub"):SmallIcon("var_pub"));
          break;
        case BrowserNode::Protected:
          setPixmap(0, node->isStatic()?SmallIcon("svar_prot"):SmallIcon("var_prot"));
          break;
        case BrowserNode::Private:
          setPixmap(0, node->isStatic()?SmallIcon("svar_priv"):SmallIcon("var_priv"));
          break;
      }      
      break;
    case BrowserNode::MethodType:
      if (!dynamic_cast<BrowserListViewItem*>(parent())->m_isClassLevel)
      {
        setPixmap(0, SmallIcon("fcall"));
      }
      else
      {
        switch(node->visibility())
        {
          case BrowserNode::Public:
            setPixmap(0, node->isStatic()?SmallIcon("sfcall_pub"):SmallIcon("fcall_pub"));
            break;
          case BrowserNode::Protected:
            setPixmap(0, node->isStatic()?SmallIcon("sfcall_prot"):SmallIcon("fcall_prot"));
            break;
          case BrowserNode::Private:
            setPixmap(0, node->isStatic()?SmallIcon("sfcall_priv"):SmallIcon("fcall_priv"));
            break;
        }
      }
      break;
  }
}

QString BrowserListViewItem::stringPath()
{
  QListViewItem* item = this;
  QString str = item->text(0);
  while((item = item->parent()) != NULL) {
    str = item->text(0) + "/" + str;
  }
  return str;
}

BrowserListViewItem* BrowserListViewItem::lastItem()
{
  QListViewItem *item =  firstChild();
  while(item)
  {
    item = item->nextSibling();
  }
  return dynamic_cast<BrowserListViewItem*>(item);
}

void BrowserListViewItem::clear()
{
  QListViewItem *item =  firstChild();
  while(item)
  {
    delete item;
    item = firstChild();
  }
}
