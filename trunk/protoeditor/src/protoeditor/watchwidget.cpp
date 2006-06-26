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

#include "watchwidget.h"
#include <qlayout.h>
#include <qlabel.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <klocale.h>
#include "watchlistview.h"

WatchWidget::WatchWidget(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  QVBoxLayout* watchTabLayout = new QVBoxLayout(this, 1, 1);

  QHBoxLayout* addWatchLayout = new QHBoxLayout(0, 6, 6);

  QLabel* addLabel = new QLabel(this);
  addLabel->setText(i18n("Watch:"));
  addWatchLayout->addWidget(addLabel);

  m_edAddWatch = new KLineEdit(this);
  m_edAddWatch->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_edAddWatch->sizePolicy().hasHeightForWidth()));
  addWatchLayout->addWidget(m_edAddWatch);

  m_btAddWatch = new KPushButton(this);
  m_btAddWatch->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_btAddWatch->sizePolicy().hasHeightForWidth()));
  m_btAddWatch->setText(i18n("Add"));
  addWatchLayout->addWidget(m_btAddWatch);

  QSpacerItem* spacer = new QSpacerItem(430, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  addWatchLayout->addItem(spacer);
  watchTabLayout->addLayout(addWatchLayout);

  m_watchList = new WatchListView(this);
  watchTabLayout->addWidget(m_watchList);
}


WatchWidget::~WatchWidget()
{
}


#include "watchwidget.moc"
