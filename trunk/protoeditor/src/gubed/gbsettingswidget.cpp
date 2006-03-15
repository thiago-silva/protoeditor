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

  QGridLayout* grid = new QGridLayout(0, 3, 3, -1, 16);

  QLabel* lblistenPort = new QLabel(this);
  lblistenPort->setText("Listen on port:");
  grid->addWidget(lblistenPort,0 ,0);

  m_spListenPort = new QSpinBox(this);
  m_spListenPort->setMaxValue(99999);
  grid->addWidget(m_spListenPort, 0, 1);  

  QLabel* lbScripts = new QLabel(this);
  lbScripts->setText("StartSession.php path:");
  
  grid->addWidget(lbScripts, 1, 0);
    
  m_edStartSessionScript = new KLineEdit(this);
  grid->addWidget(m_edStartSessionScript, 1, 1);

  m_ckEnableJIT = new QCheckBox(this);
  m_ckEnableJIT->setText("Enable JIT");
  grid->addWidget(m_ckEnableJIT, 2, 0);

  mainLayout->addLayout(grid);
        
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
//   if(str[str.length()-1] == '/')
//   {
//     str.remove(str.length()-1, 1);
//   }
  return str;
}
  
void GBSettingsWidget::populate()
{
  m_spListenPort->setValue(m_settings->listenPort());
  m_ckEnableJIT->setChecked(m_settings->enableJIT());
  m_edStartSessionScript->setText(m_settings->startSessionScript());
}

#include "gbsettingswidget.moc"
