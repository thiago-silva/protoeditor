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
 
#include "gbsettingswidget.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <klineedit.h>

#include <qlayout.h>

#include "gbsettings.h"

GBSettingsWidget::GBSettingsWidget(GBSettings* settings, QWidget *parent, const char *name)
  : DebuggerTab(parent, name), m_settings(settings)
{

  QVBoxLayout *mainLayout = new QVBoxLayout(this, 10, 16);

  QHBoxLayout* portbox = new QHBoxLayout(0, -1, 16);

  QLabel* lblistenPort = new QLabel(this);
  lblistenPort->setText("Listen on port:");
  portbox->addWidget(lblistenPort);

  m_spListenPort = new QSpinBox(this);
  m_spListenPort->setMaxValue(99999);
  portbox->addWidget(m_spListenPort);
  portbox->addItem(new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));

  mainLayout->addLayout(portbox);

  QHBoxLayout* scriptsLayout = new QHBoxLayout(0, -1, 16);
  QLabel* lbScripts = new QLabel(this);
  lbScripts->setText("StartSession.php path:");
  
  scriptsLayout->addWidget(lbScripts);
    
  m_edStartSessionScript = new KLineEdit(this);
  scriptsLayout->addWidget(m_edStartSessionScript);
  scriptsLayout->addItem(new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));

  mainLayout->addLayout(scriptsLayout);

  QHBoxLayout* jitbox = new QHBoxLayout(0, -1, 16);

  m_ckEnableJIT = new QCheckBox(this);
  m_ckEnableJIT->setText("Enable JIT");
  jitbox->addWidget(m_ckEnableJIT);
  jitbox->addItem(new QSpacerItem( 324, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));

  mainLayout->addLayout(jitbox);

        
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

QString GBSettingsWidget::startSessionScript()
{
  QString str = m_edStartSessionScript->text();
  if(str[str.length()-1] == '/')
  {
    str.remove(str.length()-1, 1);
  }
  return str;
}
  
void GBSettingsWidget::populate()
{
  m_spListenPort->setValue(m_settings->listenPort());
  m_ckEnableJIT->setChecked(m_settings->enableJIT());
  m_edStartSessionScript->setText(m_settings->startSessionScript());
}

#include "gbsettingswidget.moc"
