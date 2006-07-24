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

#include "extappsettingswidget.h"

#include "protoeditor.h"
#include "protoeditorsettings.h"
#include "extappsettings.h"

#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <klineedit.h>
#include <klocale.h>


ExtAppSettingsWidget::ExtAppSettingsWidget(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
  QVBoxLayout* vbox = new QVBoxLayout(this, 10, 10);
  QGridLayout* grid = new QGridLayout(this, 2, 2, 3, 10);

  m_ckUseExternalBrowser = new QCheckBox(this);
  m_ckUseExternalBrowser->setText(i18n("Use external browser"));

  m_cbExtBrowser = new QComboBox(this);
  m_cbExtBrowser->insertItem(i18n("Konqueror"));
  m_cbExtBrowser->insertItem(i18n("Mozilla"));
  m_cbExtBrowser->insertItem(i18n("Firefox"));
  m_cbExtBrowser->insertItem(i18n("Opera"));
  m_cbExtBrowser->setEnabled(false);

  grid->addWidget(m_ckUseExternalBrowser, 0, 0);
  grid->addWidget(m_cbExtBrowser, 0, 1);
  
  connect(m_ckUseExternalBrowser, SIGNAL(stateChanged(int)), this, SLOT(slotUseExtBrowser(int)));

  m_ckUseConsole = new QCheckBox(this);
  m_ckUseConsole->setText(i18n("External console:"));
  grid->addWidget(m_ckUseConsole, 1, 0);

  connect(m_ckUseConsole, SIGNAL(stateChanged(int)), this, SLOT(slotUseConsole(int)));

  m_edConsole = new KLineEdit(this);
  m_edConsole->setEnabled(false);
  grid->addWidget(m_edConsole, 1, 1);

  vbox->addLayout(grid);
  vbox->addItem(new QSpacerItem(0,0));  
}

void ExtAppSettingsWidget::populate()
{
  m_cbExtBrowser->setCurrentItem(Protoeditor::self()->settings()->extAppSettings()->externalBrowser());
  m_ckUseExternalBrowser->setChecked(Protoeditor::self()->settings()->extAppSettings()->useExternalBrowser());
  m_ckUseConsole->setChecked(Protoeditor::self()->settings()->extAppSettings()->useConsole());
  m_edConsole->setText(Protoeditor::self()->settings()->extAppSettings()->console());
}

void ExtAppSettingsWidget::slotUseExtBrowser(int value)
{
  if(QButton::On == value) {
    m_cbExtBrowser->setEnabled(true);
  } else {
    m_cbExtBrowser->setEnabled(false);
  }
}

void ExtAppSettingsWidget::slotUseConsole(int value)
{
  if(QButton::On == value) {
    m_edConsole->setEnabled(true);
  } else {
    m_edConsole->setEnabled(false);
  }
}

ExtAppSettingsWidget::~ExtAppSettingsWidget()
{
}

void ExtAppSettingsWidget::updateSettings()
{
  Protoeditor::self()->settings()->extAppSettings()->setExternalBrowser(m_cbExtBrowser->currentItem());

  Protoeditor::self()->settings()->extAppSettings()->setUseExternalBrowser(
    m_ckUseExternalBrowser->isChecked());

  Protoeditor::self()->settings()->extAppSettings()->setUseConsole(
    m_ckUseConsole->isChecked());

  Protoeditor::self()->settings()->extAppSettings()->setConsole(
    m_edConsole->text());
}

#include "extappsettingswidget.moc"
