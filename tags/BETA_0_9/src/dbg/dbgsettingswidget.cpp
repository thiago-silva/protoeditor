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
#include "dbgsettingswidget.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>

#include "dbgsettings.h"

DBGSettingsWidget::DBGSettingsWidget(DBGSettings* settings, QWidget *parent, const char *name)
    : DebuggerTab(parent, name), m_settings(settings) {

  QVBoxLayout *mainLayout = new QVBoxLayout(this, 16, 5);

  QHBoxLayout* jitbox = new QHBoxLayout(0, 1, 10);

  m_ckEnableJIT = new QCheckBox(this);
  m_ckEnableJIT->setText("Enable JIT");
  jitbox->addWidget(m_ckEnableJIT);

  mainLayout->addLayout(jitbox);
  QHBoxLayout* portbox = new QHBoxLayout(0, 1, 10);

  QLabel* lblistenPort = new QLabel(this);
  lblistenPort->setText("Listen on port:");
  portbox->addWidget(lblistenPort);

  m_spListenPort = new QSpinBox(this);
  m_spListenPort->setMaxValue(99999);
  portbox->addWidget(m_spListenPort);
  portbox->addItem(new QSpacerItem( 40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ));

  mainLayout->addLayout(portbox);

  QGroupBox* groupbox = new QGroupBox(this);
  groupbox->setTitle("Options");
  groupbox->setColumnLayout(0, Qt::Vertical );
  QVBoxLayout* groupboxLayout = new QVBoxLayout(groupbox->layout());
  groupboxLayout->setAlignment(Qt::AlignTop);

  QGridLayout* grid = new QGridLayout(0, 3, 2, 3, 6);

  m_ckBreakOnLoad = new QCheckBox(groupbox);
  m_ckBreakOnLoad->setText("Break on load");
  grid->addWidget(m_ckBreakOnLoad, 0, 0);

  m_ckSendErrors = new QCheckBox(groupbox);
  m_ckSendErrors->setText("Send erros");
  grid->addWidget(m_ckSendErrors, 1, 0);


  m_ckSendLogs = new QCheckBox(groupbox);
  m_ckSendLogs->setText("Send logs");
  grid->addWidget(m_ckSendLogs, 2, 0);


  m_ckSendOutput = new QCheckBox(groupbox);
  m_ckSendOutput->setText("Send output");
  grid->addWidget(m_ckSendOutput, 0, 1);

  m_ckSendDetailedOutput = new QCheckBox(groupbox);
  m_ckSendDetailedOutput->setText("Send detailed output");
  grid->addWidget(m_ckSendDetailedOutput, 1, 1);

  groupboxLayout->addLayout(grid);

  mainLayout->addWidget(groupbox);

  mainLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

DBGSettingsWidget::~DBGSettingsWidget() {}

bool DBGSettingsWidget::breakOnLoad()
{
  return m_ckBreakOnLoad->isChecked();
}

bool DBGSettingsWidget::sendErrors()
{
  return m_ckSendErrors->isChecked();
}

bool DBGSettingsWidget::sendLogs()
{
  return m_ckSendLogs->isChecked();
}

bool DBGSettingsWidget::sendOutput()
{
  return m_ckSendOutput->isChecked();
}

bool DBGSettingsWidget::sendDetailedOutput()
{
  return m_ckSendDetailedOutput->isChecked();
}

int DBGSettingsWidget::listenPort()
{
  return m_spListenPort->value();
}

bool DBGSettingsWidget::enableJIT()
{
  return m_ckEnableJIT->isChecked();
}


void DBGSettingsWidget::populate()
{
  m_ckBreakOnLoad->setChecked(m_settings->breakOnLoad());
  m_ckSendErrors->setChecked(m_settings->sendErrors());
  m_ckSendLogs->setChecked(m_settings->sendLogs());
  m_ckSendOutput->setChecked(m_settings->sendOutput());
  m_ckSendDetailedOutput->setChecked(m_settings->sendDetailedOutput());

  m_spListenPort->setValue(m_settings->listenPort());
  m_ckEnableJIT->setChecked(m_settings->enableJIT());
}

#include "dbgsettingswidget.moc"
