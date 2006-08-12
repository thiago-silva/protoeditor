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
#ifndef BROWSERNODE_H
#define BROWSERNODE_H

#include <qvaluelist.h>
#include <qstring.h>
#include <kurl.h>

class BrowserNode
{
public:
  enum { /* classes */ ClassType, InterfaceType, 
         /* members */ ConstType, AttributeType, MethodType };

  enum { Public, Protected, Private };

  BrowserNode();
  ~BrowserNode();

  void setName(const QString&);
  QString name() const;

  void setType(int type);
  int type() const;

  void setFileURL(const KURL&);
  KURL fileURL() const;

  void setLine(int);
  int line() const;

  void setVisibility(int);
  int visibility() const;

  void setStatic(bool);
  bool isStatic() const;

  void addChild(BrowserNode*);
  void setChilds(QValueList<BrowserNode*>);
  QValueList<BrowserNode*> childs() const;

private:
  QString m_name;
  int m_type;
  KURL m_url;
  int m_line;
  int m_visibility;
  bool m_static;
  QValueList<BrowserNode*> m_childs;
};

#endif
