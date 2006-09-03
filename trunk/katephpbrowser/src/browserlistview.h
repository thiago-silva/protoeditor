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
#ifndef BROWSERLISTVIEW_H
#define BROWSERLISTVIEW_H

#include <klistview.h>
#include <qptrlist.h>
#include <qmap.h>

class BrowserNode;
class KURL;
class BrowserListViewItem;

class BrowserListView : public KListView
{
  Q_OBJECT
public:
  BrowserListView(QWidget*, const char* = 0);
  ~BrowserListView();

  void addFileNode(BrowserNode*);

  QMap<QString, QStringList> folderStructure();
  void setFolderStructure(const QMap<QString, QStringList>&);

  void clear();
signals:
  void gotoFileLine(const KURL&, int);

private slots:
  void slotDoubleClick(QListViewItem*, const QPoint&, int);
  void slotContextMenuRequested(QListViewItem*, const QPoint&, int);

  void slotItemExpanded(QListViewItem*);
  void slotItemCollapsed(QListViewItem*);

protected:
  void movableDropEvent(QListViewItem*, QListViewItem*);

private:
  void addNode(BrowserNode* node, BrowserListViewItem* parent = 0);  
  BrowserListViewItem* lastRootItem();

  void reexpandItems();
  BrowserListViewItem* getItemFromPath(QString);

  void createFolder(const QString&);
  void deleteFolder(QListViewItem*);
  bool folderExists(const QString&);

//   QListViewItem* findItem(const QString&);

  QMap<QString, QStringList> m_folderStructure;

  QMap<QString, QListViewItem*> m_folders;

  QMap<KURL, QPair<BrowserListViewItem*, QPtrList<BrowserNode> > > m_map;
  QValueList<QString>    m_expanded;
};

#endif
