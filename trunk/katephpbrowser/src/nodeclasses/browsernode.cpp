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

#include "browsernode.h"

BrowserNode::BrowserNode()
  : m_name(),m_type(0), m_url(), m_line(0), m_visibility(0), m_static(false)
{
}

// BrowserNode::BrowserNode(const BrowserNode& other)
//   : m_name(other.m_name),m_type(other.m_type), m_url(other.m_url), 
//     m_line(other.m_line), m_visibility(other.m_visibility), m_static(other.m_static),
//     m_childs(other.m_childs)
// {
//   
// }

BrowserNode::~BrowserNode()
{
}

void BrowserNode::setName(const QString& name)
{
  m_name = name;
}

QString BrowserNode::name() const
{
  return m_name;
}


void BrowserNode::setType(int type)
{
  m_type = type;
}

int BrowserNode::type() const
{
  return m_type;
}

void BrowserNode::setFileURL(const KURL& url)
{
  m_url = url;
}

KURL BrowserNode::fileURL() const
{
  return m_url;
}

void BrowserNode::setLine(int line)
{
  m_line = line;
}

int BrowserNode::line() const
{
  return m_line;
}

void BrowserNode::setVisibility(int v)
{
  m_visibility = v;
}

int BrowserNode::visibility() const
{
  return m_visibility;
}

void BrowserNode::setStatic(bool value)
{
  m_static = value;
}

bool BrowserNode::isStatic() const
{
  return m_static;
}


void BrowserNode::addChild(BrowserNode* node)
{
  m_childs.append(node);
}

void BrowserNode::setChilds(QValueList<BrowserNode*> list)
{
  m_childs = list;
}

QValueList<BrowserNode*> BrowserNode::childs() const
{
  return m_childs;
}

// BrowserNode& BrowserNode::operator=(const BrowserNode& right)
// {
//   if(this == &right)
//   {
//     return *this;
//   }
// 
//   m_name = right.m_name;
//   m_type = right.m_type;
//   m_url  = right.m_url;
//   m_line = right.m_line;
//   m_visibility = right.m_visibility;
//   m_static = right.m_static;
//   m_childs = right.m_childs;
// 
//   return *this;
// }
