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
#include "phpsettingswidget.h"

#include "protoeditorsettings.h"
#include "debuggersettingsinterface.h"
#include "phpsettings.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtabwidget.h>

#include <klineedit.h>
#include <klocale.h>


PHPSettingsWidget::PHPSettingsWidget(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this, 3, 10);

  QGridLayout* grid = new QGridLayout(0, 2,2, 3, 10);

  QLabel* lbPhp= new QLabel(this);
  lbPhp->setText(i18n("PHP command:"));
  grid->addWidget(lbPhp, 0, 0);


  m_edPHPCommand = new KLineEdit(this);
  grid->addWidget(m_edPHPCommand, 0, 1);
  
  QLabel* lbDefaultDebugger = new QLabel(this);
  lbDefaultDebugger->setText(i18n("Default debugger:"));
  grid->addWidget(lbDefaultDebugger, 1, 0);

  m_cbDefaultDebugger = new QComboBox(this);
  QValueList<DebuggerSettingsInterface*> list =
      ProtoeditorSettings::self()->debuggerSettingsList();
  
  for(QValueList<DebuggerSettingsInterface*>::iterator it = list.begin(); it != list.end(); it++) {
    m_cbDefaultDebugger->insertItem((*it)->name());
  }  
  grid->addWidget(m_cbDefaultDebugger,1, 1);

  mainLayout->addLayout(grid);

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

void PHPSettingsWidget::populate()
{
  PHPSettings* settings = ProtoeditorSettings::self()->phpSettings();
  if(!settings->defaultDebugger().isEmpty())
  {
    m_cbDefaultDebugger->setCurrentText(settings->defaultDebugger());
  }
  
  m_edPHPCommand->setText(settings->PHPCommand());
  
  QValueList<DebuggerSettingsInterface*>::iterator it;
  for(it = m_debuggerSettingsList.begin(); it != m_debuggerSettingsList.end(); ++it) {
    (*it)->widget()->populate();
  }
}

void PHPSettingsWidget::updateSettings()
{
  PHPSettings* settings = ProtoeditorSettings::self()->phpSettings();

  settings->setDefaultDebugger(m_cbDefaultDebugger->currentText());
  settings->setPHPCommand(m_edPHPCommand->text());

  QValueList<DebuggerSettingsInterface*>::iterator it;
  for(it = m_debuggerSettingsList.begin(); it != m_debuggerSettingsList.end(); ++it) {
    (*it)->loadValuesFromWidget();
  }
}

#include "phpsettingswidget.moc"
