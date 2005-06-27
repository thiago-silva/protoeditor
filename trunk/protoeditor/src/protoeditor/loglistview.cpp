/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
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

#include "loglistview.h"
#include <klocale.h>
#include <kiconloader.h>
#include "debuggermanager.h"

LogListView::LogListView(QWidget *parent, const char *name)
 : KListView(parent, name)
{
  setSorting(-1);
  setAllColumnsShowFocus(true);

  addColumn("*");
  addColumn(tr2i18n("Message"));
  addColumn(tr2i18n("Line"));
  addColumn(tr2i18n("File"));

  setColumnWidth(LogListView::MessageCol, 350);
  setColumnWidth(LogListView::FileCol, 200);

  setColumnWidthMode (0, QListView::Manual);
  setColumnWidthMode (1, QListView::Manual);
  setColumnWidthMode (2, QListView::Manual);
  setColumnWidthMode (3, QListView::Manual);

  connect(this, SIGNAL(doubleClicked(QListViewItem*, const QPoint &, int )),
    this, SLOT(slotDoubleClick(QListViewItem *, const QPoint &, int )));

}

LogListView::~LogListView()
{
}

void LogListView::add(int type, QString message, int line, QString file) {
  QListViewItem* item = new QListViewItem(this);
  item->setSelectable(true);

  switch(type) {
    case DebuggerManager::InfoMsg:
      item->setPixmap(LogListView::TypeCol, SmallIcon("info"));
      break;
    case DebuggerManager::WarningMsg:
      item->setPixmap(LogListView::TypeCol, SmallIcon("messagebox_warning"));
      break;
    case DebuggerManager::ErrorMsg:
      item->setPixmap(LogListView::TypeCol, SmallIcon("cancel"));
      break;
  }

  item->setText(LogListView::MessageCol, message);
  item->setText(LogListView::LineCol, QString::number(line));
  item->setText(LogListView::FileCol, file);

  //add new item to the bottom of the list
  item->moveItem(lastItem());
}

void LogListView::slotDoubleClick(QListViewItem * item, const QPoint &, int)
{
  emit sigDoubleClick(item->text(LogListView::FileCol)
                    , item->text(LogListView::LineCol).toLong());
}


#include "loglistview.moc"
