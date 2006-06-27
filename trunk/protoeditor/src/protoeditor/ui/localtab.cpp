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

#include "localtab.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>

#include "combostack.h"
#include "variablelistview.h"

LocalTab::LocalTab(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  QVBoxLayout* varTabLayout = new QVBoxLayout(this, 1, 1);
  QHBoxLayout* stackComboLayout = new QHBoxLayout(0, 6, 6);

  QLabel* stackLabel = new QLabel(this);
  stackLabel->setText(i18n("Stack:"));
  stackLabel->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, stackLabel->sizePolicy().hasHeightForWidth()));
  stackComboLayout->addWidget(stackLabel);

  m_comboStack = new ComboStack(this);
  stackComboLayout->addWidget(m_comboStack);
  varTabLayout->addLayout(stackComboLayout);

  m_localVarList= new VariableListView(this);
  varTabLayout->addWidget(m_localVarList);
}


LocalTab::~LocalTab()
{
}

VariableListView* LocalTab::localVarList()
{
  return m_localVarList;
}

ComboStack* LocalTab::comboStack()
{
  return m_comboStack;
}

#include "localtab.moc"
