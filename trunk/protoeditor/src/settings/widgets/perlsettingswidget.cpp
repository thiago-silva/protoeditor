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
#include <qtabwidget.h>

#include <klineedit.h>
#include <klocale.h>


PerlSettingsWidget::PerlSettingsWidget(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this, 3, 10);

  QGridLayout* grid = new QGridLayout(0, 2,2, 3, 10);

  QLabel* lbPhp= new QLabel(this);
  lbPhp->setText(i18n("Perl command:"));
  grid->addWidget(lbPhp, 0, 0);


  m_edPerlCommand = new KLineEdit(this);
  grid->addWidget(m_edPerlCommand, 0, 1);
  
  mainLayout->addLayout(grid);
}


PerlSettingsWidget::~PerlSettingsWidget()
{
}

void PerlSettingsWidget::populate()
{
  PerlSettings* settings = ProtoeditorSettings::self()->perlSettings();
  m_edPerlCommand->setText(settings->PerlCommand());
}

void PerlSettingsWidget::updateSettings()
{
  PerlSettings* settings = ProtoeditorSettings::self()->perlSettings();

  settings->setPerlCommand(m_edPerlCommand->text());
}

#include "perlsettingswidget.moc"
