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
#include <kurlrequester.h>

SiteSettingsDialog::SiteSettingsDialog(QWidget *parent, const char *name)
    : KDialogBase(KDialogBase::Plain, WStyle_DialogBorder, parent, name, true, "Site settings", KDialogBase::Ok|KDialogBase::Cancel)
{

  QFrame *frame = plainPage();
  QVBoxLayout* vlayout = new QVBoxLayout(frame, 0, 0);

  QGridLayout* grid = new QGridLayout(0, 5, 2, 3, 5);

  QLabel* label = new QLabel(frame);
  label->setText("Site name:");
  grid->addWidget(label, 0, 0);

  m_edName = new KLineEdit(frame);
  grid->addWidget(m_edName, 0, 1);

  label = new QLabel(frame);
  label->setText("Host:");
  grid->addWidget(label, 1, 0);

  m_edHost = new KLineEdit(frame);
  grid->addWidget(m_edHost, 1, 1);

  label = new QLabel(frame);
  label->setText("Port:");
  grid->addWidget(label, 2, 0);

  m_spPort = new QSpinBox(frame);
  m_spPort->setMaxValue(999999);
  m_spPort->setValue(80);
  grid->addWidget(m_spPort, 2, 1);

  label = new QLabel(frame);
  label->setText("Remote base dir:");
  grid->addWidget(label, 3, 0);

  m_edRemoteBaseDir = new KLineEdit(frame);
  grid->addWidget(m_edRemoteBaseDir, 3, 1);

  label = new QLabel(frame);
  label->setText("Local base dir:");
  grid->addWidget(label, 4, 0);

  m_edLocalBaseDir = new KURLRequester(frame);
  m_edLocalBaseDir->setMode(KFile::Directory);
  grid->addWidget(m_edLocalBaseDir, 4, 1);
  
  //
  label = new QLabel(frame);
  label->setText("Default file:");
  grid->addWidget(label, 5, 0);

  m_edDefaultFile = new KURLRequester(frame);
  //note: this filter must be the same as in void MainWindow::slotOpenFile()
  m_edDefaultFile->setFilter("*.php| PHP Scripts\n*|All Files");
  grid->addWidget(m_edDefaultFile, 5, 1);

  vlayout->addLayout(grid);
  vlayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

  enableButtonSeparator(true);

  setMinimumWidth(400);
}


SiteSettingsDialog::~SiteSettingsDialog()
{}

void SiteSettingsDialog::setUpdate()
{
  m_edName->setEnabled(false);
}

void SiteSettingsDialog::slotOk()
{
  if(m_edName->text().isEmpty())
  {
    KMessageBox::sorry(this, "\"Name\" is required.");
  }
  else
  {
    KDialogBase::slotOk();
  }
}

void SiteSettingsDialog::populate(const QString& name, const QString& host, int port,
                                  const QString& remoteBaseDir, const QString& localBaseDir,
                                  const QString& defaultFile)
{
  m_edName->setText(name);
  m_edHost->setText(host);
  m_spPort->setValue(port);
  m_edRemoteBaseDir->setText(remoteBaseDir);
  m_edLocalBaseDir->setURL(localBaseDir);
  m_edDefaultFile->setURL(defaultFile);
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
  return m_edLocalBaseDir->url();
}

QString SiteSettingsDialog::defaultFile()
{
  return m_edDefaultFile->url();
}

#include "sitesettingsdialog.moc"
