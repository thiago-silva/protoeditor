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
#include "phpsettingswidget.h"

#include "protoeditorsettings.h"
#include "debuggersettingsinterface.h"
#include "phpsettings.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtabwidget.h>

#include <klineedit.h>


PHPSettingsWidget::PHPSettingsWidget(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this, 3, 10);

  /*
  QHBoxLayout* phpbox = new QHBoxLayout(0, 3, 10);

  QLabel* lbPhp= new QLabel(this);
  lbPhp->setText("PHP binary path:");
  phpbox->addWidget(lbPhp);


  m_edPHPBinaryPath = new KLineEdit(this);
  m_edPHPBinaryPath->setText("/usr/local/bin/php");
  phpbox->addWidget(m_edPHPBinaryPath);

  mainLayout->addLayout(phpbox);

  QHBoxLayout* debuggerbox = new QHBoxLayout(0, 3, 10);

  QLabel* lbDefaultDebugger = new QLabel(this);
  lbDefaultDebugger->setText("Default debugger:");
  debuggerbox->addWidget(lbDefaultDebugger);

  m_cbDefaultDebugger = new QComboBox(this);
  debuggerbox->addWidget(m_cbDefaultDebugger);

  mainLayout->addLayout(debuggerbox);
  */

  QTabWidget* debuggersTabWidget = new QTabWidget(this);

  m_debuggerSettingsList =
    ProtoeditorSettings::self()->debuggerSettingsList();

  QValueList<DebuggerSettingsInterface*>::iterator it;
  for(it = m_debuggerSettingsList.begin(); it != m_debuggerSettingsList.end(); ++it) {
    debuggersTabWidget->addTab((*it)->widget(), (*it)->name());
  }

  mainLayout->addWidget(debuggersTabWidget);
}


PHPSettingsWidget::~PHPSettingsWidget()
{
}

void PHPSettingsWidget::updateSettings()
{
  //PHPSettings* settings = ProtoeditorSettings::self()->phpSettings();
  //settings->setDefaultDebugger(m_cbDefaultDebugger->currentItem());

  QValueList<DebuggerSettingsInterface*>::iterator it;
  for(it = m_debuggerSettingsList.begin(); it != m_debuggerSettingsList.end(); ++it) {
    (*it)->loadValuesFromWidget();
  }
}

#include "phpsettingswidget.moc"
