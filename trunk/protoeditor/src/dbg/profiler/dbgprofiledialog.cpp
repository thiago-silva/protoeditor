/***************************************************************************
 *   Copyright (C) 2004 by Thiago Silva                                    *
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
 
#include "dbgprofiledialog.h"
#include "dbgprofilelistview.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qtoolbutton.h> 

DBGProfileDialog::DBGProfileDialog(QWidget* parent, const char* name)
  : KDialog(parent, name)
{
  setCaption("DBG Profiler");

  QVBoxLayout* vlayout = new QVBoxLayout(this, 11, 6);

  QHBoxLayout* hbox = new QHBoxLayout(0, 0, 6);

  QLabel* lbView = new QLabel(this);
  lbView->setText("View mode:");
  hbox->addWidget(lbView);

  m_btModule = new QToolButton(this);
  m_btModule->setText("Module");
  m_btModule->setToggleButton(true);
  hbox->addWidget(m_btModule);

  m_btContext = new QToolButton(this);
  m_btContext->setText("Context");
  m_btContext->setToggleButton(true);
  hbox->addWidget(m_btContext);

  m_btDetail = new QToolButton(this);
  m_btDetail->setText("Detailed");
  m_btDetail->setToggleButton(true);
  hbox->addWidget(m_btDetail);

  QSpacerItem* spacer = new QSpacerItem( 285, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  hbox->addItem(spacer);

  vlayout->addLayout(hbox);

  m_listView = new DBGProfileListView(this);
  vlayout->addWidget(m_listView);

  resize(600, 200);
}

DBGProfileDialog::~DBGProfileDialog()
{

}

void DBGProfileDialog::closeEvent(QCloseEvent * e)
{
  emit sigClose();
  KDialog::closeEvent(e);
}

void DBGProfileDialog::clear()
{
  m_listView->clear();
}

void DBGProfileDialog::addData(DBGProfileData* data)
{
  m_listView->addData(data);
}

#include "dbgprofiledialog.moc"
