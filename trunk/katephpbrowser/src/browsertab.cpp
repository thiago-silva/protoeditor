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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "browsertab.h"
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <kiconloader.h>

#include <kate/application.h>
#include <kate/mainwindow.h>
#include <kate/view.h>
#include <kate/viewmanager.h>
#include <kate/documentmanager.h>

#include "browserlistview.h"
#include "schemaconfigurationdlg.h"
#include "schemasettings.h"

#include "browserloader.h"

BrowserTab::BrowserTab(Kate::Application *app, QWidget* parent,const char* name)
  : QWidget(parent, name), m_app(app)
{
  m_settings = new SchemaSettings();

  m_loader = new BrowserLoader(this);

  QVBoxLayout* layout = new QVBoxLayout(this, 1, 1);
  
  QHBoxLayout* hbox = new QHBoxLayout(0);

  m_cbSchema = new QComboBox(this);
  hbox->addWidget(m_cbSchema);

  m_btConfig = new QToolButton(this);
  m_btConfig->setIconSet(SmallIcon("gear"));  
  m_btConfig->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_btConfig->sizePolicy().hasHeightForWidth()));
  hbox->addWidget(m_btConfig);

  layout->addLayout(hbox);

  m_browserList = new BrowserListView(this);  
  layout->addWidget(m_browserList);

  connect(m_browserList, SIGNAL(gotoFileLine(const KURL&, int)),
    this, SLOT(slotGotoFileLine(const KURL&, int)));

  connect(m_cbSchema, SIGNAL(activated(int)),
    this, SLOT(slotSchemaChanged(int)));

  connect(m_btConfig, SIGNAL(clicked()), this, SLOT(slotConfigureSchemas()));

  reloadSettings();
  reloadBrowser();
}

BrowserTab::~BrowserTab()
{
  delete m_settings;
}

void BrowserTab::reloadSettings()
{
  //remembe the current item
  QString current = m_cbSchema->currentText();

  m_cbSchema->clear();

  QMap<QString, Schema> map = m_settings->schemas();  
  QMap<QString, Schema>::iterator it;
  for(it = map.begin(); it != map.end(); ++it)   
  {
    m_cbSchema->insertItem(it.key());
  }

  //sets the current item
  for(int i = 0; i < m_cbSchema->count(); ++i)
  {
    if(m_cbSchema->text(i) == current)
    {
      m_cbSchema->setCurrentText(current);
    }
  }

  //update the current item settings
  m_settings->setCurrentSchemaName(m_cbSchema->currentText());  
}

void BrowserTab::slotConfigureSchemas()
{
  SchemaConfigurationDialog* dialog = new SchemaConfigurationDialog(this);
  dialog->populate(m_settings);

  if(dialog->exec() == QDialog::Accepted) 
  {
    m_settings->setSchemas(dialog->schemas());
    m_settings->setCurrentSchemaName(dialog->currentSchemaName());
    m_settings->writeConfig();

    reloadSettings();
    reloadBrowser();
  }  

  delete dialog;  
}

BrowserListView* BrowserTab::listView()
{
  return m_browserList;
}

void BrowserTab::slotGotoFileLine(const KURL& url, int line)
{
  activateDocument(url);
  m_app->activeMainWindow()->viewManager()->activeView()->setCursorPosition(line-1, 0);
}

void BrowserTab::slotSchemaChanged(int)
{
  m_settings->setCurrentSchemaName(m_cbSchema->currentText());
  reloadBrowser();
}

void BrowserTab::reloadBrowser()
{
  m_browserList->clear();
  m_loader->clear();
  if(m_settings->schemas().count() > 0)
  {
    m_loader->update(m_settings->schema(m_settings->currentSchemaName()));
  }
}

void BrowserTab::activateDocument(const KURL& url)
{
  int c = m_app->documentManager()->findDocument(url);
  if(c != -1)
  {
    m_app->activeMainWindow()->viewManager()->activateView(c);
  }
  else
  {
    m_app->activeMainWindow()->viewManager()->openURL(url);
  }
}

#include "browsertab.moc"
