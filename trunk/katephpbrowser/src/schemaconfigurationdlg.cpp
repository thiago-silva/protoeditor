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
  : KDialogBase(KDialogBase::Plain, WStyle_DialogBorder, parent, name, true, i18n("Schema configuration"), KDialogBase::Ok|KDialogBase::Cancel)
{
  QFrame *frame = plainPage();

  QVBoxLayout* mainLayout = new QVBoxLayout(frame, 11, 6);

  QHBoxLayout* hbox = new QHBoxLayout(0, 0, 6);

  QLabel* lb = new QLabel(frame);
  lb->setText(i18n("Schemas:"));
  lb->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, lb->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(lb);

  m_cbSchemas = new QComboBox(frame);
  hbox->addWidget(m_cbSchemas);

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
  lb->setText(i18n("Schema name:"));
  lb->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, lb->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(lb);
  m_edName = new KLineEdit(frame);
  hbox->addWidget(m_edName);
  
  mainLayout->addLayout(hbox);

  m_schemaListView = new KListView(frame);
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

  m_btRemove = new QPushButton(frame);
  m_btRemove->setText(i18n("Remove"));
  hbox->addWidget(m_btRemove);

//   m_btClear = new QPushButton(frame);
//   m_btClear->setText(i18n("Clear"));
//   hbox->addWidget(m_btClear);

  hbox->addItem(new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
  mainLayout->addLayout(hbox);

  enableButtonSeparator(true);
  resize( QSize(400, 400).expandedTo(minimumSizeHint()) );

  connect(m_btAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
  connect(m_btRemove, SIGNAL(clicked()), this, SLOT(slotRemove()));
//   connect(m_btClear, SIGNAL(clicked()), this, SLOT(slotClear()));

  connect(m_cbSchemas, SIGNAL(activated(int)), 
    this, SLOT(slotSchemaChanged(int)));  

  connect(m_btDeleteSchema, SIGNAL(clicked()), this, SLOT(slotDeleteSchema()));

  connect(m_btSaveSchema, SIGNAL(clicked()), this, SLOT(slotSaveSchema()));
  
  connect(m_schemaListView, SIGNAL(selectionChanged()), this, 
      SLOT(slotSelectionChanged()));

  m_btRemove->setEnabled(false);
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
    QListViewItem* item = new QListViewItem(m_schemaListView);
    item->setText(0, KURL::fromPathOrURL(m_urlDirectory->url()).prettyURL());

    m_schema.setDirectoryList(URLValues());    
    m_urlDirectory->clear();

    if(m_map.contains(m_schema.name()))
    {
      m_map[m_schema.name()] = m_schema;
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
  QListViewItem* item = m_schemaListView->selectedItem();

  m_schema.setDirectoryList(URLValues());

  delete item;  

  m_schemaListView->clearSelection();
  m_urlDirectory->clear();

  m_schema.setDirectoryList(URLValues()); 

  if(m_map.contains(m_schema.name()))
  {
    m_map[m_schema.name()] = m_schema;
  }
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
  QListViewItem* item = m_schemaListView->selectedItem();

  if(item)
  {
    m_btRemove->setEnabled(true);
  } 
  else
  {
    m_btRemove->setEnabled(false);
  }
}

void SchemaConfigurationDialog::slotSaveSchema()
{  
  m_schema = Schema(m_edName->text(), URLValues());
  m_map[m_schema.name()] = m_schema;

  for(int i = 0; i < m_cbSchemas->count(); ++i)
  {
    if(m_cbSchemas->text(i) == m_schema.name())
    {
      return;
    }
  }
  m_cbSchemas->insertItem(m_schema.name());
}

void SchemaConfigurationDialog::slotDeleteSchema()
{
  int idx = m_cbSchemas->currentItem();
  if(idx != -1)
  {
    m_map.remove(m_cbSchemas->currentText());
    m_cbSchemas->removeItem(idx);
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

// void SchemaConfigurationDialog::slotOk()
// {
//   Schema s;
// 
//   s.setName(m_cbSchemas->currentText());
//   
//   QListViewItem* item = m_schemaListView->firstChild();
//   while(item)
//   {
//     s.addDirectory(KURL::fromPathOrURL(item->text(0)));
//     item = item->nextSibling();
//   }
//   KDialogBase::slotOk();
// }

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
