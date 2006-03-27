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
#include "sitesettingsdialog.h"

#include <qlabel.h>
#include <qcombobox.h>
#include <klineedit.h>
#include <qlayout.h>
#include <qframe.h>
#include <kfiledialog.h>

#include <kmessagebox.h>
#include <kurlrequester.h>
#include <qtabwidget.h>
#include <klistview.h>

#include "debuggersettingsinterface.h"
#include "sitesettings.h"
#include "protoeditorsettings.h"


DirMappingWidget::DirMappingWidget(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this, 10, 10);

  m_listView = new KListView(this);
  mainLayout->addWidget(m_listView);

  QGridLayout* grid = new QGridLayout(0, 2, 2, 3, 5);

  QLabel* label = new QLabel(this);
  label->setText("Local dir:");
  grid->addWidget(label, 0, 0);

  m_edLocalDir = new KURLRequester(this);
  m_edLocalDir->setMode(KFile::Directory);
  grid->addWidget(m_edLocalDir, 0, 1);

  label = new QLabel(this);
  label->setText("Remote dir:");
  grid->addWidget(label, 1, 0);

  m_edRemoteDir = new KLineEdit(this);
  grid->addWidget(m_edRemoteDir, 1, 1);


  mainLayout->addLayout(grid);

  QHBoxLayout* hbox = new QHBoxLayout(this, 0, 3);

  m_btAdd = new QPushButton(this);
  m_btAdd->setText("Add");
  hbox->addWidget(m_btAdd);

  m_btUpdate = new QPushButton(this);
  m_btUpdate->setText("Update");
  hbox->addWidget(m_btUpdate);

  m_btRemove = new QPushButton(this);
  m_btRemove->setText("Remove");
  hbox->addWidget(m_btRemove);

  mainLayout->addLayout(hbox);

  initListView();

  connect(m_btAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
  connect(m_btUpdate, SIGNAL(clicked()), this, SLOT(slotUpdate()));
  connect(m_btRemove, SIGNAL(clicked()), this, SLOT(slotRemove()));
  
  connect(m_listView, SIGNAL(selectionChanged()), this, 
      SLOT(slotSelectionChanged()));

  m_btRemove->setEnabled(false);
  m_btUpdate->setEnabled(false);
}

DirMappingWidget::~DirMappingWidget()
{
}

void DirMappingWidget::initListView()
{
//   m_listView->setSorting(-1);
  m_listView->setAllColumnsShowFocus(true);

  m_listView->addColumn("Local dir");  
  m_listView->addColumn("Remote dir");

  m_listView->setResizeMode(QListView::AllColumns);
}

void DirMappingWidget::setMappings(const QMap<QString,QString>& mappings)
{
  QMap<QString, QString>::const_iterator it;
  for(it = mappings.begin(); it != mappings.end(); ++it) 
  {
    addMapping(it.key(), it.data());
  }
}

QMap<QString,QString> DirMappingWidget::mappings()
{
  QMap<QString, QString> mappings;

  QListViewItem* item = m_listView->firstChild();
  while(item)
  {
    mappings[item->text(LocalColumn)] = item->text(RemoteColumn);
    item = item->nextSibling();
  }
  return mappings;
}

void DirMappingWidget::slotAdd()
{
  KURL local;
  local.setDirectory(m_edLocalDir->url()); 

  KURL remote;
  remote.setDirectory(m_edRemoteDir->text());

  if(m_edLocalDir->url().isEmpty() || !local.isValid())
  {
    KMessageBox::sorry(this, "\"Local dir\" is not valid.");
    return;
  }

  if(m_edRemoteDir->text().isEmpty() || !remote.isValid())
  {
    KMessageBox::sorry(this, "\"Remote dir\" is not valid.");
    return;
  }

  QString l = m_edLocalDir->url();
  if(!l.endsWith("/"))
  {
    l += "/";
  }

  if(itemExists(l))
  {
    KMessageBox::sorry(this, "\"Local dir\" already exists.");
    return;
  }

  addMapping(m_edLocalDir->url(), m_edRemoteDir->text());

  m_listView->clearSelection();

  m_edLocalDir->clear();
  m_edRemoteDir->clear();
}

void DirMappingWidget::addMapping(QString local, QString remote)
{
  QListViewItem* item = new QListViewItem(m_listView);

  item->setText(LocalColumn, local);
  item->setText(RemoteColumn, remote);
}

void DirMappingWidget::slotUpdate()
{
  QListViewItem* item = m_listView->selectedItem();

  QString local = m_edLocalDir->url();
  QString remote = m_edRemoteDir->text();
  if(!local.endsWith("/"))
  {
    local += "/";     
  }

  if(!remote.endsWith("/"))
  {
    remote += "/";
  }
  
  item->setText(LocalColumn, local);
  item->setText(RemoteColumn, remote);

  m_listView->clearSelection();

  m_edLocalDir->clear();
  m_edRemoteDir->clear();
}

void DirMappingWidget::slotRemove()
{
  QListViewItem* item = m_listView->selectedItem();
  delete item;

  m_listView->clearSelection();

  m_edLocalDir->clear();
  m_edRemoteDir->clear();
}

void DirMappingWidget::slotSelectionChanged()
{
//   KMessageBox::sorry(this, "\"Local dir\" already exists.");

  QListViewItem* item = m_listView->selectedItem();

  if(item)
  {
    m_edLocalDir->setURL(item->text(LocalColumn));
    m_edRemoteDir->setText(item->text(RemoteColumn)); 

    m_btRemove->setEnabled(true);
    m_btUpdate->setEnabled(true);
  } 
  else
  {
    m_btRemove->setEnabled(false);
    m_btUpdate->setEnabled(false);
  }
}

bool DirMappingWidget::itemExists(const QString& local)
{
  QListViewItem* item = m_listView->firstChild();
  while(item)
  {
    if(item->text(LocalColumn) == local)
    {
      return true;
    }
    item = item->nextSibling();
  }
  return false;
}

//-----------------------------------------------------------------

SiteSettingsDialog::SiteSettingsDialog(QWidget *parent, const char *name)
    : KDialogBase(KDialogBase::Plain, WStyle_DialogBorder, parent, name, true, "Site settings", KDialogBase::Ok|KDialogBase::Cancel)
{
  QFrame *frame = plainPage();

  QVBoxLayout* mainLayout = new QVBoxLayout(frame, 0, 0);

  QTabWidget* tab = new QTabWidget(frame);

  QWidget* sitewidget = new QWidget(tab);
  
  QVBoxLayout* siteLayout = new QVBoxLayout(sitewidget, 10, 0);
  
  QGridLayout* grid = new QGridLayout(0, 5, 2, 3, 5);

  QLabel* label = new QLabel(sitewidget);
  label->setText("Site name:");
  grid->addWidget(label, 0, 0);

  m_edName = new KLineEdit(sitewidget);
  grid->addWidget(m_edName, 0, 1);

  label = new QLabel(sitewidget);
  label->setText("URL:");
  grid->addWidget(label, 1, 0);

  m_edUrl = new KLineEdit(sitewidget);
  m_edUrl->setText("http://");
  grid->addWidget(m_edUrl, 1, 1);

  label = new QLabel(sitewidget);
  label->setText("Remote base dir:");
  grid->addWidget(label, 2, 0);

  m_edRemoteBaseDir = new KLineEdit(sitewidget);
  grid->addWidget(m_edRemoteBaseDir, 2, 1);

  label = new QLabel(sitewidget);
  label->setText("Local base dir:");
  grid->addWidget(label, 3, 0);

  m_edLocalBaseDir = new KURLRequester(sitewidget);
  m_edLocalBaseDir->setMode(KFile::Directory);
  grid->addWidget(m_edLocalBaseDir, 3, 1);
  
  //
  label = new QLabel(sitewidget);
  label->setText("Default file:");
  grid->addWidget(label, 4, 0);

  m_edDefaultFile = new KURLRequester(sitewidget);
  grid->addWidget(m_edDefaultFile, 4, 1);

  label = new QLabel(sitewidget);
  label->setText("Debugger:");
  grid->addWidget(label, 5, 0);

  m_cbDebuggerClient = new QComboBox(sitewidget);
  QValueList<DebuggerSettingsInterface*> list =
      ProtoeditorSettings::self()->debuggerSettingsList();
  
  for(QValueList<DebuggerSettingsInterface*>::iterator it = list.begin(); it != list.end(); it++) {
    m_cbDebuggerClient->insertItem((*it)->name());
  }
  
  grid->addWidget(m_cbDebuggerClient, 5, 1);
    
  siteLayout->addLayout(grid);
  siteLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

  tab->addTab(sitewidget, "Site");



  m_mappingWidget = new DirMappingWidget(tab);
  tab->addTab(m_mappingWidget, "Mappings");

  mainLayout->addWidget(tab);  

  enableButtonSeparator(true);

  resize(600, 400);  

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
                                  const KURL& defaultFile, const QString& debuggerClient,
                                  QMap<QString,QString>& mappings)
{
  m_edName->setText(name);
  m_edUrl->setText(url.url());
  m_edRemoteBaseDir->setText(remoteBaseDir.pathOrURL());
  m_edLocalBaseDir->setURL(localBaseDir.pathOrURL());
  m_edDefaultFile->setURL(defaultFile.pathOrURL());
  m_cbDebuggerClient->setCurrentText(debuggerClient);
  m_mappingWidget->setMappings(mappings);
}


QString SiteSettingsDialog::name()
{
  return m_edName->text();
}

KURL SiteSettingsDialog::url()
{
  if(m_edUrl->text().isEmpty())
  {
    return KURL();
  }
  else
  {
    return KURL::fromPathOrURL(m_edUrl->text());
  }
}

KURL SiteSettingsDialog::remoteBaseDir()
{
  if(m_edRemoteBaseDir->text().isEmpty())
  {
    return KURL();
  }
  else
  {
    KURL url;
    url.setDirectory(m_edRemoteBaseDir->text()); 
    return url;
  }
}

KURL SiteSettingsDialog::localBaseDir()
{
  if(m_edLocalBaseDir->url().isEmpty())
  {
    return KURL();
  }
  else
  {
    KURL url;
    url.setDirectory(m_edLocalBaseDir->url()); 
    return url;
  }
}

KURL SiteSettingsDialog::defaultFile()
{
  if(m_edDefaultFile->url().isEmpty())
  {
    return KURL();
  }
  else
  {
    return KURL::fromPathOrURL(m_edDefaultFile->url());
  }
}

QString SiteSettingsDialog::debuggerClient()
{
  return m_cbDebuggerClient->currentText();
}

QMap<QString,QString> SiteSettingsDialog::mappings()
{
  return m_mappingWidget->mappings();
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
