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
#ifndef DBGSETTINGSWIDGET_H
#define DBGSETTINGSWIDGET_H

#include "debuggersettingsinterface.h"

class QCheckBox;
class QSpinBox;

class DBGSettings;

class DBGSettingsWidget : public DebuggerTab {
  Q_OBJECT
public:
  DBGSettingsWidget(DBGSettings*, QWidget *parent = 0, const char *name = 0);
  ~DBGSettingsWidget();

  bool breakOnLoad();
  bool sendErrors();
  bool sendLogs();
  bool sendOutput();
  bool sendDetailedOutput();
  int  listenPort();
  bool enableJIT();

  void populate();
private:

  QCheckBox *m_ckBreakOnLoad;
  QCheckBox *m_ckSendErrors;
  QCheckBox *m_ckSendLogs;
  QCheckBox *m_ckSendOutput;
  QCheckBox *m_ckSendDetailedOutput;

  QSpinBox  *m_spListenPort;
  QCheckBox *m_ckEnableJIT;

  DBGSettings* m_settings;
};

#endif
