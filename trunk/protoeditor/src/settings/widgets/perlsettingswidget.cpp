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
#include "perlsettingswidget.h"

#include "protoeditorsettings.h"
#include "debuggersettingsinterface.h"
#include "perlsettings.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qtabwidget.h>

#include <klineedit.h>
#include <klocale.h>


PerlSettingsWidget::PerlSettingsWidget(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this, 3, 10);

  QGridLayout* grid = new QGridLayout(0, 3,2, 3, 10);

  m_ckEnabled = new QCheckBox(this);
  m_ckEnabled->setText(i18n("Enabled"));
  grid->addWidget(m_ckEnabled, 0, 0);

  QLabel* lbPhp= new QLabel(this);
  lbPhp->setText(i18n("Perl command:"));
  grid->addWidget(lbPhp, 1, 0);


  m_edPerlCommand = new KLineEdit(this);
  grid->addWidget(m_edPerlCommand, 1, 1);
  
  mainLayout->addLayout(grid);

  connect(m_ckEnabled, SIGNAL(stateChanged(int)), this, SLOT(slotLangEnabled(int)));
  slotLangEnabled(false);
}


PerlSettingsWidget::~PerlSettingsWidget()
{
}

void PerlSettingsWidget::populate()
{
  PerlSettings* settings = 
    dynamic_cast<PerlSettings*>(ProtoeditorSettings::self()->languageSettings(PerlSettings::lang));


  m_ckEnabled->setChecked(settings->isEnabled());
  m_edPerlCommand->setText(settings->PerlCommand());
}

void PerlSettingsWidget::updateSettings()
{
  PerlSettings* settings = 
    dynamic_cast<PerlSettings*>(ProtoeditorSettings::self()->languageSettings(PerlSettings::lang));


  settings->setEnabled(m_ckEnabled->isChecked());
  settings->setPerlCommand(m_edPerlCommand->text());
}

void PerlSettingsWidget::slotLangEnabled(int)
{
  m_edPerlCommand->setEnabled(m_ckEnabled->isChecked());
}

#include "perlsettingswidget.moc"
