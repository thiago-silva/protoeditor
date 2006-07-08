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
#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <kdialogbase.h>

class SiteSettingsWidget;
class ExtAppSettingsWidget;
class LanguageSettings;
class LanguageSettingsWidget;

class ConfigDlg : public KDialogBase
{
  Q_OBJECT
public:
  ConfigDlg(QWidget *parent = 0, const char *name = 0);
  ~ConfigDlg();

  void showDialog();  
  
protected slots:
  void slotOk();
private:
  void populateWidgets();
  static ConfigDlg* m_self;

  SiteSettingsWidget           *m_siteSettingsWidget;
  ExtAppSettingsWidget         *m_extAppSettingsWidget;
  QValueList<LanguageSettingsWidget*> m_langSettingsWidgets;
};

#endif
