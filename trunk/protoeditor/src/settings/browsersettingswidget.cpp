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
#include "browsersettingswidget.h"
#include <qlayout.h>
//#include <klineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>

BrowserSettingsWidget::BrowserSettingsWidget(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
  QVBoxLayout* vbox = new QVBoxLayout(this, 0, 10);
  QHBoxLayout* hbox = new QHBoxLayout(this, 0, 3);

  m_ckUseExternalBrowser = new QCheckBox(this, "kcfg_UseExternalBrowser");
  m_ckUseExternalBrowser->setText("Use external browser");

  m_lbBrowserCmd = new QLabel(this);
  //m_lbBrowserCmd->setText("Browser command: ");
  m_lbBrowserCmd->setText("Browser:");

  //m_edBrowserCmd = new KLineEdit(this, "kcfg_BrowserCmd");
  //m_edBrowserCmd->setEnabled(false);
  m_cbBrowser = new QComboBox(this, "kcfg_Browser");
  m_cbBrowser->insertItem("Konqueror");
  m_cbBrowser->insertItem("Mozilla");
  m_cbBrowser->insertItem("Firefox");
  m_cbBrowser->insertItem("Opera");
  m_cbBrowser->setEnabled(false);

  hbox->addWidget(m_lbBrowserCmd);
  hbox->addWidget(m_cbBrowser);

  vbox->addWidget(m_ckUseExternalBrowser);
  vbox->addLayout(hbox);
  vbox->addItem(new QSpacerItem(0,0));

  connect(m_ckUseExternalBrowser, SIGNAL(stateChanged(int)), this, SLOT(slotUseBrowser(int)));
}

void BrowserSettingsWidget::slotUseBrowser(int value)
{
  if(QButton::On == value) {
    m_cbBrowser->setEnabled(true);
  } else {
    m_cbBrowser->setEnabled(false);
  }
}

BrowserSettingsWidget::~BrowserSettingsWidget()
{
}


#include "browsersettingswidget.moc"
