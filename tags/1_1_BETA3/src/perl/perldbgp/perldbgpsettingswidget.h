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

#ifndef PERLSETTINGSWIDGET_H
#define PERLSETTINGSWIDGET_H

#include "debuggersettingsinterface.h"

class QCheckBox;
class KLineEdit;
class QSpinBox;

class PerlDBGPSettings;


class PerlDBGPSettingsWidget : public DebuggerTab
{
  Q_OBJECT
public:
  PerlDBGPSettingsWidget(PerlDBGPSettings*, QWidget *parent = 0, const char *name = 0);
  ~PerlDBGPSettingsWidget();

  int  listenPort();
  bool enableJIT();
  bool sendSuperGlobals();
  bool breakOnLoad();

  QString perlLibPath();
  QString debuggerPath();

  void setLangEnabled(bool);
  void populate();
private:
  QSpinBox  *m_spListenPort;
  QCheckBox *m_ckEnableJIT;
  QCheckBox *m_ckBreakOnLoad;
  QCheckBox *m_ckSendSuperGlobals;

  KLineEdit* m_edPerlLibPath;
  KLineEdit* m_edDebuggerPath;

  PerlDBGPSettings* m_settings;

};

#endif
