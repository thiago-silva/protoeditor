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
#ifndef DEBUGGERSETTINGSWIDGET_H
#define DEBUGGERSETTINGSWIDGET_H

#include <qwidget.h>

class QCheckBox;
class KLineEdit;
class QSpinBox;
class QGridLayout;
class QLabel;

class QTabWidget;
class QButtonGroup;
class QRadioButton;
class QVBoxLayout;

class DebuggerSettingsWidget : public QWidget
{
  Q_OBJECT
public:
  DebuggerSettingsWidget(QWidget *parent = 0, const char *name = 0);
  ~DebuggerSettingsWidget();

  QTabWidget   *m_tabWidget;
  QButtonGroup *m_buttonGroup;
  QRadioButton *m_rdDBG;
  QVBoxLayout  *m_vlayout;
  QRadioButton *m_rdOther;


  //QCheckBox    *m_ckDBGnabled;
  KLineEdit    *m_edLocalBaseDir;
  KLineEdit    *m_edRemoteBaseDir;
  QSpinBox     *m_spListenPort;
  KLineEdit    *m_edHost;

  QGridLayout *m_gridLayout;
  //QLabel      *m_lbDBGEnabled;
  QLabel      *m_lbLocalBaseDir;
  QLabel      *m_lbRemoteBaseDir;
  QLabel      *m_lbListenPort;
  QLabel      *m_lbHost;

private slots:
  void clientChanged(int);

private:
  void enableDBG();
  void disableDBG();
};

#endif
