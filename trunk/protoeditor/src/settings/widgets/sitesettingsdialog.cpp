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
#include "sitesettingsdialog.h"

#include <qlabel.h>
#include <qspinbox.h>
#include <klineedit.h>
#include <qlayout.h>
#include <qframe.h>

#include <kmessagebox.h>


SiteSettingsDialog::SiteSettingsDialog(QWidget *parent, const char *name)
 : KDialogBase(KDialogBase::Plain, WStyle_DialogBorder, parent, name, true, "Site settings", KDialogBase::Ok|KDialogBase::Cancel)
{

  QFrame *frame = plainPage();
  QVBoxLayout* vlayout = new QVBoxLayout(frame, 0, 0);

  QGridLayout* grid = new QGridLayout(0, 5, 2, 3, 5);

  m_lbName = new QLabel(frame);
  m_lbName->setText("Site name:");
  grid->addWidget(m_lbName, 0, 0);

  m_edName = new KLineEdit(frame);
  grid->addWidget(m_edName, 0, 1);

  m_lbHost = new QLabel(frame);
  m_lbHost->setText("Host:");
  grid->addWidget(m_lbHost, 1, 0);

  m_edHost = new KLineEdit(frame);
  grid->addWidget(m_edHost, 1, 1);

  m_lbPort = new QLabel(frame);
  m_lbPort->setText("Port:");
  grid->addWidget(m_lbPort, 2, 0);


  m_spPort = new QSpinBox(frame);
  m_spPort->setMaxValue(999999);
  m_spPort->setValue(80);
  grid->addWidget(m_spPort, 2, 1);

  m_lbRemoteBaseDir = new QLabel(frame);
  m_lbRemoteBaseDir->setText("Remote base dir:");
  grid->addWidget(m_lbRemoteBaseDir, 3, 0);

  m_edRemoteBaseDir = new KLineEdit(frame);
  grid->addWidget(m_edRemoteBaseDir, 3, 1);

  m_lbLocalBaseDir = new QLabel(frame);
  m_lbLocalBaseDir->setText("Local base dir:");
  grid->addWidget(m_lbLocalBaseDir, 4, 0);

  m_edLocalBaseDir = new KLineEdit(frame);
  grid->addWidget(m_edLocalBaseDir, 4, 1);

  vlayout->addLayout(grid);
  vlayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

  enableButtonSeparator(true);
}


SiteSettingsDialog::~SiteSettingsDialog()
{
}

void SiteSettingsDialog::setUpdate()
{
  m_edName->setEnabled(false);
}

void SiteSettingsDialog::slotOk()
{
  if(m_edName->text().isEmpty()) {
    KMessageBox::sorry(this, "\"Name\" is required.");
  } else {
    KDialogBase::slotOk();
  }
}

void SiteSettingsDialog::populate(const QString& name, const QString& host, int port,
                const QString& remoteBaseDir, const QString& localBaseDir)
{
  m_edName->setText(name);
  m_edHost->setText(host);
  m_spPort->setValue(port);
  m_edRemoteBaseDir->setText(remoteBaseDir);
  m_edLocalBaseDir->setText(localBaseDir);
}


QString SiteSettingsDialog::name()
{
  return m_edName->text();
}

QString SiteSettingsDialog::host()
{
  return m_edHost->text();
}

int SiteSettingsDialog::port()
{
  return m_spPort->value();
}

QString SiteSettingsDialog::remoteBaseDir()
{
  return m_edRemoteBaseDir->text();
}

QString SiteSettingsDialog::localBaseDir()
{
  return m_edLocalBaseDir->text();
}

#include "sitesettingsdialog.moc"
