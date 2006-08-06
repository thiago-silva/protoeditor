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
#ifndef SCHEMACONFIGURATION_H
#define SCHEMACONFIGURATION_H

#include <kdialogbase.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <kurl.h>

#include "schema.h"

class QComboBox;
class KListView;
class QPushButton;
class QPushButton;
class KURLRequester;
class QToolButton;
class QPushButton;
class KLineEdit;

class SchemaSettings;

class SchemaConfigurationDialog : public KDialogBase
{
  Q_OBJECT
public:
  SchemaConfigurationDialog(QWidget*, const char* = 0);
  ~SchemaConfigurationDialog();

  void populate(SchemaSettings*);
  QValueList<Schema> schemas();

  QString currentSchemaName();
private slots:
  void slotSelectionChanged();
  void slotAdd();
  void slotRemove();
  void slotClear();

  void slotSaveSchema();
  void slotDeleteSchema();

  void slotSchemaChanged(int);

protected slots:
//   void slotOk();
private:
  void loadCurrentSchema();
  bool directoryExists(const QString&);

  QValueList<KURL> URLValues();

  QComboBox* m_cbSchemas;
  QToolButton* m_btSaveSchema;
  QToolButton* m_btDeleteSchema;  
  KListView* m_schemaListView;
  QPushButton* m_btAdd;
  QPushButton* m_btRemove;
  QPushButton* m_btClear;
  KURLRequester* m_urlDirectory;
  KLineEdit* m_edName;

  Schema m_schema;
  QMap<QString, Schema> m_map;
};

#endif
