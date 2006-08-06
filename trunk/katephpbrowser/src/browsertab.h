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
#ifndef BROWSERTAB_H
#define BROWSERTAB_H

#include <qwidget.h>

class QComboBox;
class QToolButton;
class BrowserListView;

class SchemaSettings;

class BrowserTab : public QWidget
{
  Q_OBJECT
public:
  BrowserTab(QWidget*, const char* = 0);
  ~BrowserTab();

protected slots:
  void slotConfigureSchemas();

  SchemaSettings* m_settings;

private:
  QComboBox* m_cbSchema;
  QToolButton* m_btConfig;
  BrowserListView* m_browserList;

  void reloadSettings();
};

#endif
