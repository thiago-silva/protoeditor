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
#include "debuggersettingswidget.h"
#include "debuggersettings.h"

#include <qlayout.h>
#include <klineedit.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <klocale.h>
#include <qcheckbox.h>

#include <qtabwidget.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

DebuggerSettingsWidget::DebuggerSettingsWidget(QWidget *parent, const char *name)
 : QWidget(parent, name)
{

  m_vlayout = new QVBoxLayout(this, 3, 20);

  m_buttonGroup = new QButtonGroup(this, "kcfg_Client");
  m_buttonGroup->setMinimumHeight(m_buttonGroup->height()+10);

  QHBoxLayout* groupLayout = new QHBoxLayout(m_buttonGroup, 10, 10);

  m_rdDBG = new QRadioButton(m_buttonGroup);
  m_rdDBG->setText("DBG");
  m_buttonGroup->insert(m_rdDBG);
  groupLayout->addWidget(m_rdDBG);

  m_rdOther = new QRadioButton(m_buttonGroup);
  m_rdOther->setText("Other");
  m_buttonGroup->insert(m_rdOther);
  groupLayout->addWidget(m_rdOther);


  m_tabWidget = new QTabWidget(this);
  QWidget* dbgTab = new QWidget(m_tabWidget);

  m_gridLayout = new QGridLayout(dbgTab, 4, 2, 5, 5);

  m_edLocalBaseDir = new KLineEdit(dbgTab, "kcfg_LocalBaseDir");
  m_edServerBaseDir = new KLineEdit(dbgTab, "kcfg_ServerBaseDir");
  m_spListenPort = new QSpinBox(dbgTab, "kcfg_ListenPort");
  m_spListenPort->setMaxValue(9999);
  m_edServerHost = new KLineEdit(dbgTab, "kcfg_ServerHost");

  m_lbLocalBaseDir = new QLabel(dbgTab);
  m_lbLocalBaseDir->setText(i18n("Local basedir:"));
  m_lbServerBaseDir = new QLabel(dbgTab);
  m_lbServerBaseDir->setText(i18n("Server basedir:"));
  m_lbListenPort = new QLabel(dbgTab);
  m_lbListenPort->setText(i18n("Listen port:"));
  m_lbServerHost = new QLabel(dbgTab);
  m_lbServerHost->setText(i18n("Server Host:"));

  m_gridLayout->addWidget(m_lbLocalBaseDir, 0, 0);
  m_gridLayout->addWidget(m_edLocalBaseDir, 0, 1);
  m_gridLayout->addWidget(m_lbServerBaseDir, 1, 0);
  m_gridLayout->addWidget(m_edServerBaseDir, 1, 1);
  m_gridLayout->addWidget(m_lbListenPort, 2, 0);
  m_gridLayout->addWidget(m_spListenPort, 2, 1);
  m_gridLayout->addWidget(m_lbServerHost, 3, 0);
  m_gridLayout->addWidget(m_edServerHost, 3, 1);

  m_vlayout->addWidget(m_buttonGroup);
  //m_vlayout->addLayout(groupLayout);
  m_vlayout->addWidget(m_tabWidget);

  m_tabWidget->insertTab(dbgTab, "DBG");

  disableDBG();

  connect(m_rdDBG, SIGNAL(stateChanged(int)), this, SLOT(clientChanged(int)));
  //connect(m_rdOther, SIGNAL(stateChanged(int)), this, SLOT(clientChanged(int)));
}


DebuggerSettingsWidget::~DebuggerSettingsWidget()
{
}

void DebuggerSettingsWidget::clientChanged(int)
{
  switch(m_buttonGroup->selectedId()) {
    case DebuggerSettings::EnumClient::DBG:
      enableDBG();
      break;
    default:
      disableDBG();
  }
}

void DebuggerSettingsWidget::enableDBG()
{
  m_edLocalBaseDir->setEnabled(true);
  m_edServerBaseDir->setEnabled(true);
  m_spListenPort->setEnabled(true);
  m_edServerHost->setEnabled(true);
}

void DebuggerSettingsWidget::disableDBG()
{
  m_edLocalBaseDir->setEnabled(false);
  m_edServerBaseDir->setEnabled(false);
  m_spListenPort->setEnabled(false);
  m_edServerHost->setEnabled(false);
}

#include "debuggersettingswidget.moc"
