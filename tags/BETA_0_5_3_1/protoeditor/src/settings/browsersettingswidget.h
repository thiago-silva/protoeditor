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
#ifndef BROWSERSETTINGSWIDGET_H
#define BROWSERSETTINGSWIDGET_H

#include <qwidget.h>

class QCheckBox;
class QLabel;
class KLineEdit;

class BrowserSettingsWidget : public QWidget
{
  Q_OBJECT
public:
  BrowserSettingsWidget(QWidget *parent = 0, const char *name = 0);
  ~BrowserSettingsWidget();

  QCheckBox *m_ckUseExternalBrowser;

  QLabel    *m_lbBrowserCmd;
  KLineEdit *m_edBrowserCmd;

private slots:
  void slotUseBrowser(int);
};

#endif
