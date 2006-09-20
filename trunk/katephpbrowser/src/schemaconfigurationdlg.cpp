/***************************************************************************
 *   Copyright (C) 2006 by Thiago Silva                                    *
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
 *   along with frame program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "schemaconfigurationdlg.h"
#include <kurlrequester.h>
#include <klistview.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <klocale.h>
#include <kiconloader.h>
#include <qheader.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include "schemasettings.h"


SchemaConfigurationDialog::SchemaConfigurationDialog(QWidget* parent, const char* name)
  : KDialogBase(KDialogBase::Plain, WStyle_DialogBorder, parent, name, true, i18n("Browser configuration"), KDialogBase::Ok|KDialogBase::Cancel)
{
  QFrame *frame = plainPage();

  QVBoxLayout* mainLayout = new QVBoxLayout(frame, 11, 6);

  QHBoxLayout* hbox = new QHBoxLayout(0, 0, 6);

  QLabel* lb = new QLabel(frame);
  lb->setText(i18n("Resource:"));
  lb->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, lb->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(lb);

  m_cbSchemas = new QComboBox(frame);
  hbox->addWidget(m_cbSchemas);

  
  m_btNewSchema = new QToolButton(frame);
  m_btNewSchema->setIconSet(SmallIcon("filenew"));
  m_btNewSchema->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, lb->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(m_btNewSchema);

  m_btSaveSchema = new QToolButton(frame);
  m_btSaveSchema->setIconSet(SmallIcon("filesave"));
  m_btSaveSchema->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, lb->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(m_btSaveSchema);


  m_btDeleteSchema = new QToolButton(frame);
  m_btDeleteSchema->setIconSet(SmallIcon("editdelete"));
  m_btDeleteSchema->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, lb->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(m_btDeleteSchema);
  
  mainLayout->addLayout(hbox);

  hbox = new QHBoxLayout(0, 1, 1);

  lb = new QLabel(frame);
  lb->setText(i18n("Resource name:"));
  lb->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, lb->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(lb);
  m_edName = new KLineEdit(frame);
  hbox->addWidget(m_edName);
  
  mainLayout->addLayout(hbox);

  m_schemaListView = new KListView(frame);
  m_schemaListView->setSelectionMode(QListView::Extended);
  m_schemaListView->addColumn("");
  m_schemaListView->header()->hide();
  m_schemaListView ->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  mainLayout->addWidget(m_schemaListView);  

  hbox = new QHBoxLayout(0, 1, 1);

  lb = new QLabel(frame);
  lb->setText(i18n("Directory:"));
  lb->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, lb->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(lb);

  m_urlDirectory = new KURLRequester(frame);
  m_urlDirectory->setMode(KFile::Directory);
  hbox->addWidget(m_urlDirectory);

  mainLayout->addLayout(hbox);

  hbox = new QHBoxLayout(0, 1, 1);

  m_btAdd = new QPushButton(frame);
  m_btAdd->setText(i18n("Add"));
  hbox->addWidget(m_btAdd);

  m_btAddRecursive = new QPushButton(frame);
  m_btAddRecursive->setText(i18n("Add recursive"));
  hbox->addWidget(m_btAddRecursive);
  

  m_btRemove = new QPushButton(frame);
  m_btRemove->setText(i18n("Remove"));
  hbox->addWidget(m_btRemove);

//   m_btClear = new QPushButton(frame);
//   m_btClear->setText(i18n("Clear"));
//   hbox->addWidget(m_btClear);

  hbox->addItem(new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
  mainLayout->addLayout(hbox);

  enableButtonSeparator(true);

  connect(m_btAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
  connect(m_btAddRecursive, SIGNAL(clicked()), this, SLOT(slotAddRecursive()));
  connect(m_btRemove, SIGNAL(clicked()), this, SLOT(slotRemove()));
//   connect(m_btClear, SIGNAL(clicked()), this, SLOT(slotClear()));

  connect(m_cbSchemas, SIGNAL(activated(int)), 
    this, SLOT(slotSchemaChanged(int)));  

  
  connect(m_btNewSchema, SIGNAL(clicked()), this, SLOT(slotNewSchema()));

  connect(m_btSaveSchema, SIGNAL(clicked()), this, SLOT(slotSaveSchema()));

  connect(m_btDeleteSchema, SIGNAL(clicked()), this, SLOT(slotDeleteSchema()));
  
  connect(m_schemaListView, SIGNAL(selectionChanged()), this, 
      SLOT(slotSelectionChanged()));

  m_btRemove->setEnabled(false);

  resize(QSize(590, 300).expandedTo(minimumSizeHint()));
}

SchemaConfigurationDialog::~SchemaConfigurationDialog()
{
}

void SchemaConfigurationDialog::populate(SchemaSettings* settings)
{
  m_map = settings->schemas();

  if(m_map.contains(settings->currentSchemaName()))
  {
    m_schema = m_map[settings->currentSchemaName()];
  }

//   m_edName->setText(m_schema.name());

  QMap<QString, Schema>::iterator it;
  for(it = m_map.begin(); it != m_map.end(); ++it) 
  {
    m_cbSchemas->insertItem(it.key());
  }  
  loadCurrentSchema();
}

QValueList<Schema> SchemaConfigurationDialog::schemas()
{  
  return m_map.values();
}

void SchemaConfigurationDialog::slotAdd()
{
  if(m_urlDirectory->url().isEmpty())
  {
    KMessageBox::sorry(this, i18n("Invalid URL"));
  }
  else if(directoryExists(m_urlDirectory->url()))
  {
    KMessageBox::sorry(this, i18n("URL already added"));
  }
  else  
  {
    addDirectory(m_urlDirectory->url());
    m_urlDirectory->clear();
  }
}

void SchemaConfigurationDialog::addDirectory(const QString& path)
{
  QListViewItem* item = new QListViewItem(m_schemaListView);
  item->setText(0, KURL::fromPathOrURL(path).prettyURL());
}

void SchemaConfigurationDialog::slotAddRecursive()
{
  if(m_urlDirectory->url().isEmpty())
  {
    KMessageBox::sorry(this, i18n("Invalid URL"));
  }
  else if(directoryExists(m_urlDirectory->url()))
  {
    KMessageBox::sorry(this, i18n("URL already added"));
  }
  else
  {
    addDirectory(m_urlDirectory->url());
    addRecursive(m_urlDirectory->url());
    m_urlDirectory->clear();
  }
}

void SchemaConfigurationDialog::addRecursive(const QString& path)
{
  QDir dir(path);
  if(dir.exists() && dir.isReadable())
  {
    dir.setFilter(QDir::Dirs);
    QStringList list = dir.entryList();
    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) 
    {
      QDir other(*it);
      if(dir.isReadable() && ((*it) != ".") && ((*it) != ".."))
      {
        addDirectory(path + "/" + other.path());
        addRecursive(path + "/" + other.path());
      }
    }
  }
}

QValueList<KURL> SchemaConfigurationDialog::URLValues()
{
  QValueList<KURL> list;

  QListViewItem* item = m_schemaListView->firstChild();
  while(item)
  {
    list.append(KURL::fromPathOrURL(item->text(0)));
    item = item->nextSibling();
  }
  return list;
}

bool SchemaConfigurationDialog::directoryExists(const QString& dir)
{
  QListViewItem* item = m_schemaListView->firstChild();
  while(item)
  {
    if(item->text(0) == KURL::fromPathOrURL(dir).prettyURL())
    {
      return true;
    }
    item = item->nextSibling();
  }
  return false;  
}

void SchemaConfigurationDialog::slotRemove()
{
  QPtrList<QListViewItem>  items = m_schemaListView->selectedItems();

  QListViewItem* item;
  for(item = items.first(); item; item = items.next())
  {
    delete item;
  }

  m_schema.setDirectoryList(URLValues());

  m_schemaListView->clearSelection();

//   if(m_map.contains(m_schema.name()))
//   {
//     m_map[m_schema.name()] = m_schema;
//   }
}

void SchemaConfigurationDialog::slotClear()
{
  m_edName->clear();
  m_schemaListView->clear();
  m_urlDirectory->clear();
  m_cbSchemas->setCurrentItem(-1);
  m_schema = Schema();
}

void SchemaConfigurationDialog::slotSelectionChanged()
{
  if(m_schemaListView->selectedItems().count() != 0)
  {
    m_btRemove->setEnabled(true);
  } 
  else
  {
    m_btRemove->setEnabled(false);
  }
}


void SchemaConfigurationDialog::slotNewSchema()
{
  slotClear();
}

void SchemaConfigurationDialog::slotSaveSchema()
{
  QString oldname = m_schema.name();
  if(m_schemaListView->childCount() == 0)
  {
    KMessageBox::sorry(this, i18n("Resource has no directories"));
    return;
  }

  if(m_edName->text().isEmpty())
  {
    KMessageBox::sorry(this, i18n("Resource has no name"));
    return;
  }

  if(oldname.isEmpty() && m_map.contains(m_edName->text()))
  {
    KMessageBox::sorry(this, i18n("Resource \"") + m_edName->text() + i18n("\" already exists"));
    return;
  }

  m_schema.setName(m_edName->text());
  m_schema.setDirectoryList(URLValues());

  if(!oldname.isEmpty())
  {
    m_map.remove(m_schema.name());
    m_map[m_schema.name()] = m_schema;
    for(int i = 0; i < m_cbSchemas->count(); ++i)
    {
      if(m_cbSchemas->text(i) == oldname)
      {
        m_cbSchemas->changeItem(m_schema.name(), i);
        m_cbSchemas->setCurrentItem(i);
        break;
      }
    }
  }
  else
  {
    m_map[m_schema.name()] = m_schema;
    m_cbSchemas->insertItem(m_schema.name());    
    m_cbSchemas->setCurrentItem(m_cbSchemas->count()-1);
  }  
}

void SchemaConfigurationDialog::slotDeleteSchema()
{
  int idx = m_cbSchemas->currentItem();
  if(idx != -1)
  {
    m_map.remove(m_cbSchemas->currentText());
    m_cbSchemas->removeItem(idx);    
    m_cbSchemas->setCurrentItem(m_cbSchemas->count()-1);
  }
  
  if(m_cbSchemas->count() > 0)
  {
    m_schema = m_map[m_cbSchemas->currentText()];
    loadCurrentSchema();
  }
  else
  {
    slotClear();
  }
}


void SchemaConfigurationDialog::slotSchemaChanged(int)
{
  loadCurrentSchema();
}

void SchemaConfigurationDialog::loadCurrentSchema()
{ 
  m_schemaListView->clear();

  if(m_cbSchemas->count() == 0) return;

  if(!m_map.contains(currentSchemaName())) return;  

  m_schema = m_map[m_cbSchemas->currentText()];

  m_edName->setText(m_schema.name());

  DirectoryList_t dirlist = m_schema.directoryList();

  QListViewItem* item;

  DirectoryList_t::iterator it;
  for(it = dirlist.begin(); it != dirlist.end(); ++it) {
    item = new QListViewItem(m_schemaListView);
    item->setText(0, (*it).prettyURL());
  }
}

void SchemaConfigurationDialog::slotOk()
{
  slotSaveSchema();
  KDialogBase::slotOk();
}

QString SchemaConfigurationDialog::currentSchemaName()
{
  return m_cbSchemas->currentText();
/*
  int idx = m_cbSchemas->currentItem();
  if(!m_map.contains(idx))
  {
    return QString::null;
  }
  else
  {
    return m_map[idx].name();
  }*/
}

#include "schemaconfigurationdlg.moc"
