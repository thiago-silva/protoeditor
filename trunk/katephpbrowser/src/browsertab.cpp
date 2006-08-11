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

#include "browserlistview.h"
#include "schemaconfigurationdlg.h"
#include "schemasettings.h"

BrowserTab::BrowserTab(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  m_settings = new SchemaSettings();

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

  connect(m_btConfig, SIGNAL(clicked()), this, SLOT(slotConfigureSchemas()));

  reloadSettings();
}

BrowserTab::~BrowserTab()
{
  delete m_settings;
}

void BrowserTab::reloadSettings()
{
  m_cbSchema->clear();

  QMap<QString, Schema> map = m_settings->schemas();  
  QMap<QString, Schema>::iterator it;
  for(it = map.begin(); it != map.end(); ++it)   
  {
    m_cbSchema->insertItem(it.key());
  }
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

    updateBrowser();
  }

  reloadSettings();

  delete dialog;  
}

#include "PHPNodeWalker.hpp"
#include <qfile.h>
#include "phpbrowserparser.h"
// #include "nodebuilder.h"
#include <iostream>
void BrowserTab::updateBrowser()
{
  QFile file("/home/jester/t.php");
  file.open( IO_ReadOnly );
  PHPBrowserParser parser;
  
  antlr::RefAST ast = parser.parseText(file.readAll());
  if(ast)
  {
    std::cerr << ast->toStringTree() << std::endl << std::endl;

    PHPNodeWalker walker;
    QValueList<BrowserNode*> list = walker.start(ast);
    m_browserList->loadNodes(list);
  }
}

#include "browsertab.h"
