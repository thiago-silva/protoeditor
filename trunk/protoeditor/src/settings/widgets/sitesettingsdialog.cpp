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
#include "sitesettingsdialog.h"

#include <qlabel.h>
#include <qcombobox.h>
#include <klineedit.h>
#include <qlayout.h>
#include <qframe.h>
#include <kfiledialog.h>

#include <kmessagebox.h>
#include <kurlrequester.h>

#include "debuggersettingsinterface.h"
#include "sitesettings.h"
#include "protoeditorsettings.h"

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
  label->setText("URL:");
  grid->addWidget(label, 1, 0);

  m_edUrl = new KLineEdit(frame);
  m_edUrl->setText("http://");
  grid->addWidget(m_edUrl, 1, 1);

  label = new QLabel(frame);
  label->setText("Remote base dir:");
  grid->addWidget(label, 2, 0);

  m_edRemoteBaseDir = new KLineEdit(frame);
  grid->addWidget(m_edRemoteBaseDir, 2, 1);

  label = new QLabel(frame);
  label->setText("Local base dir:");
  grid->addWidget(label, 3, 0);

  m_edLocalBaseDir = new KURLRequester(frame);
  m_edLocalBaseDir->setMode(KFile::Directory);
  grid->addWidget(m_edLocalBaseDir, 3, 1);
  
  //
  label = new QLabel(frame);
  label->setText("Default file:");
  grid->addWidget(label, 4, 0);

  m_edDefaultFile = new KURLRequester(frame);
  grid->addWidget(m_edDefaultFile, 4, 1);

  label = new QLabel(frame);
  label->setText("Debugger:");
  grid->addWidget(label, 5, 0);

  m_cbDebuggerClient = new QComboBox(frame);
  QValueList<DebuggerSettingsInterface*> list =
      ProtoeditorSettings::self()->debuggerSettingsList();
  
  for(QValueList<DebuggerSettingsInterface*>::iterator it = list.begin(); it != list.end(); it++) {
    m_cbDebuggerClient->insertItem((*it)->name());
  }
  
  grid->addWidget(m_cbDebuggerClient, 5, 1);
    
  vlayout->addLayout(grid);
  vlayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

  enableButtonSeparator(true);

  resize(600, 250);  

  connect(m_edDefaultFile, SIGNAL(openFileDialog(KURLRequester*)),
    this, SLOT(slotOpenFileDialog(KURLRequester*)));
}


SiteSettingsDialog::~SiteSettingsDialog()
{}

void SiteSettingsDialog::setUpdate()
{
  m_edName->setEnabled(false);
}

void SiteSettingsDialog::slotOk()
{
  if(name().isEmpty())
  {
    KMessageBox::sorry(this, "\"Name\" is required.");
    return;
  }

  if(url().isEmpty() || !url().isValid())
  {
    KMessageBox::sorry(this, "\"URL\" is not valid.");
    return;
  }

  if(remoteBaseDir().isEmpty() || !remoteBaseDir().isValid())
  {
    KMessageBox::sorry(this, "\"Remote base dir\" is not valid.");
    return;
  }

  if(localBaseDir().isEmpty() || !localBaseDir().isValid())
  {
    KMessageBox::sorry(this, "\"Local base dir\" is not valid.");
    return;
  }

  if(!defaultFile().isEmpty() && !defaultFile().isValid())
  {
    KMessageBox::sorry(this, "\"Default file\" is not valid.");
    return;
  }

  KDialogBase::slotOk();
}

void SiteSettingsDialog::populate(const QString& name, const KURL& url,
                                  const KURL& remoteBaseDir, const KURL& localBaseDir,
                                  const KURL& defaultFile, const QString& debuggerClient)
{
  m_edName->setText(name);
  m_edUrl->setText(url.url());
  m_edRemoteBaseDir->setText(remoteBaseDir.pathOrURL());
  m_edLocalBaseDir->setURL(localBaseDir.pathOrURL());
  m_edDefaultFile->setURL(defaultFile.pathOrURL());
  m_cbDebuggerClient->setCurrentText(debuggerClient);
}


QString SiteSettingsDialog::name()
{
  return m_edName->text();
}

KURL SiteSettingsDialog::url()
{
  return KURL::fromPathOrURL(m_edUrl->text());
}

KURL SiteSettingsDialog::remoteBaseDir()
{
  KURL url;
  url.setDirectory(m_edRemoteBaseDir->text()); 
  return url;
}

KURL SiteSettingsDialog::localBaseDir()
{
  KURL url;
  url.setDirectory(m_edLocalBaseDir->url()); 
  return url;
}

KURL SiteSettingsDialog::defaultFile()
{
  return KURL::fromPathOrURL(m_edDefaultFile->url());
}

QString SiteSettingsDialog::debuggerClient()
{
  return m_cbDebuggerClient->currentText();
}

void SiteSettingsDialog::slotOpenFileDialog(KURLRequester* /*default file*/)
{
  //note: this filter must be the same as in void MainWindow::slotOpenFile()
  m_edDefaultFile->setFilter("*.php| PHP Scripts\n*|All Files");
  if(!m_edLocalBaseDir->url().isEmpty())
  {
    m_edDefaultFile->fileDialog()->setURL(m_edLocalBaseDir->url());
  }
}

#include "sitesettingsdialog.moc"
