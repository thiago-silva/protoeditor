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
 
#include "gbsettingswidget.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <klineedit.h>

#include <qlayout.h>
#include <qgroupbox.h>

#include "gbsettings.h"

GBSettingsWidget::GBSettingsWidget(GBSettings* settings, QWidget *parent, const char *name)
  : DebuggerTab(parent, name), m_settings(settings)
{

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

  QVBoxLayout* vbox = new QVBoxLayout(0, 3, 16);
  m_ckBreakOnLoad = new QCheckBox(groupbox);
  m_ckBreakOnLoad->setText("Break on load");
  vbox->addWidget(m_ckBreakOnLoad);
  
  
  QLabel *lbStartSession = new QLabel(groupbox);
  lbStartSession->setText("StartSession.php path:");
  vbox->addWidget(lbStartSession);

  m_edStartSessionScript = new KLineEdit(groupbox);
  vbox->addWidget(m_edStartSessionScript);

  groupboxLayout->addLayout(vbox);

  mainLayout->addWidget(groupbox);

  mainLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

GBSettingsWidget::~GBSettingsWidget() {}

int GBSettingsWidget::listenPort()
{
  return m_spListenPort->value();
}

bool GBSettingsWidget::enableJIT()
{
  return m_ckEnableJIT->isChecked();
}

bool GBSettingsWidget::breakOnLoad() {
  return m_ckBreakOnLoad->isChecked();
}

QString GBSettingsWidget::startSessionScript()
{
  QString str = m_edStartSessionScript->text();
  return str;
}
  
void GBSettingsWidget::populate()
{
  m_spListenPort->setValue(m_settings->listenPort());
  m_ckEnableJIT->setChecked(m_settings->enableJIT());
  m_edStartSessionScript->setText(m_settings->startSessionScript());
  m_ckBreakOnLoad->setChecked(m_settings->breakOnLoad());
}

#include "gbsettingswidget.moc"
