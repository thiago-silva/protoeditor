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
 
#include "xdsettingswidget.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <klocale.h>

#include "xdsettings.h"

XDSettingsWidget::XDSettingsWidget(XDSettings* settings, QWidget *parent, const char *name)
    : DebuggerTab(parent, name), m_settings(settings)
{

  QVBoxLayout *mainLayout = new QVBoxLayout(this, 16, 5);

  QHBoxLayout* jitbox = new QHBoxLayout(0, 1, 10);

  m_ckEnableJIT = new QCheckBox(this);
  m_ckEnableJIT->setText(i18n("Enable JIT"));
  jitbox->addWidget(m_ckEnableJIT);

  mainLayout->addLayout(jitbox);
  QHBoxLayout* portbox = new QHBoxLayout(0, 1, 10);

  QLabel* lblistenPort = new QLabel(this);
  lblistenPort->setText(i18n("Listen on port:"));
  portbox->addWidget(lblistenPort);

  m_spListenPort = new QSpinBox(this);
  m_spListenPort->setMaxValue(99999);
  portbox->addWidget(m_spListenPort);
  portbox->addItem(new QSpacerItem( 40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ));

  mainLayout->addLayout(portbox);

  QGroupBox* groupbox = new QGroupBox(this);
  groupbox->setTitle(i18n("Options"));
  groupbox->setColumnLayout(0, Qt::Vertical );
  QVBoxLayout* groupboxLayout = new QVBoxLayout(groupbox->layout());
  groupboxLayout->setAlignment(Qt::AlignTop);

  QVBoxLayout* vbox = new QVBoxLayout(0, 3, 6);
  m_ckBreakOnLoad = new QCheckBox(groupbox);
  m_ckBreakOnLoad->setText(i18n("Break on load"));
  vbox->addWidget(m_ckBreakOnLoad);
  
    
  m_ckSendSuperGlobals = new QCheckBox(groupbox);
  m_ckSendSuperGlobals->setText(i18n("Send super global variables"));
  vbox->addWidget(m_ckSendSuperGlobals);

  groupboxLayout->addLayout(vbox);

  mainLayout->addWidget(groupbox);

  mainLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

XDSettingsWidget::~XDSettingsWidget() {}

int XDSettingsWidget::listenPort()
{
  return m_spListenPort->value();
}

bool XDSettingsWidget::enableJIT()
{
  return m_ckEnableJIT->isChecked();
}

bool XDSettingsWidget::sendSuperGlobals() {
  return m_ckSendSuperGlobals->isChecked();
}

bool XDSettingsWidget::breakOnLoad() {
  return m_ckBreakOnLoad->isChecked();
}

void XDSettingsWidget::populate()
{
  m_spListenPort->setValue(m_settings->listenPort());
  m_ckEnableJIT->setChecked(m_settings->enableJIT());
  m_ckSendSuperGlobals->setChecked(m_settings->sendSuperGlobals());
  m_ckBreakOnLoad->setChecked(m_settings->breakOnLoad());
}

#include "xdsettingswidget.moc"
