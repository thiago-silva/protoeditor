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

#include "messagelistview.h"

#include "debuggermanager.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kurl.h>


MessageListView::MessageListView(QWidget *parent, const char *name)
 : KListView(parent, name)
{
  setSorting(-1);
  setAllColumnsShowFocus(true);

  addColumn("*");
  addColumn(tr2i18n("Message"));
  addColumn(tr2i18n("Line"));
  addColumn(tr2i18n("File"));

  setColumnWidth(MessageListView::MessageCol, 350);
  setColumnWidth(MessageListView::FileCol, 200);

  setColumnWidthMode (0, QListView::Manual);
  setColumnWidthMode (1, QListView::Manual);
  setColumnWidthMode (2, QListView::Manual);
  setColumnWidthMode (3, QListView::Manual);

  connect(this, SIGNAL(doubleClicked(QListViewItem*, const QPoint &, int )),
    this, SLOT(slotDoubleClick(QListViewItem *, const QPoint &, int )));

}

MessageListView::~MessageListView()
{
}

void MessageListView::add(int type, QString message, int line, const KURL& url) {
  QListViewItem* item = new QListViewItem(this);
  item->setSelectable(true);

  switch(type) {
    case DebuggerManager::InfoMsg:
      item->setPixmap(MessageListView::TypeCol, SmallIcon("info"));
      break;
    case DebuggerManager::WarningMsg:
      item->setPixmap(MessageListView::TypeCol, SmallIcon("messagebox_warning"));
      break;
    case DebuggerManager::ErrorMsg:
      item->setPixmap(MessageListView::TypeCol, SmallIcon("cancel"));
      break;
  }

  item->setText(MessageListView::MessageCol, message);
  item->setText(MessageListView::LineCol, QString::number(line));
  item->setText(MessageListView::FileCol, url.prettyURL());

  //add new item to the bottom of the list
  item->moveItem(lastItem());
}

void MessageListView::slotDoubleClick(QListViewItem * item, const QPoint &, int)
{
  emit sigDoubleClick(KURL::fromPathOrURL(item->text(MessageListView::FileCol))
                    , item->text(MessageListView::LineCol).toLong());
}


#include "messagelistview.moc"
