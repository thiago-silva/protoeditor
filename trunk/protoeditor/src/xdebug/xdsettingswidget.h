/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
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

#ifndef XDSETTINGSWIDGET_H
#define XDSETTINGSWIDGET_H

#include "debuggersettingsinterface.h"

class QCheckBox;
class QSpinBox;

class XDSettings;


class XDSettingsWidget : public DebuggerTab
{
  Q_OBJECT
public:
  XDSettingsWidget(XDSettings*, QWidget *parent = 0, const char *name = 0);
  ~XDSettingsWidget();

  int  listenPort();
  bool enableJIT();

  void populate();
private:
  QSpinBox  *m_spListenPort;
  QCheckBox *m_ckEnableJIT;

  XDSettings* m_settings;

};

#endif
