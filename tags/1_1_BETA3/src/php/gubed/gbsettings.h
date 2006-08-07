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
#ifndef GBSETTINGS_H
#define GBSETTINGS_H

#include "debuggersettingsinterface.h"

class GBSettingsWidget;

class GBSettings : public DebuggerSettingsInterface
{
public:
  GBSettings(const QString&, const QString& name, const QString& label, LanguageSettings*);
  ~GBSettings();

  int listenPort() const
  {
    return mListenPort;
  }

  bool enableJIT() const
  {
    return mEnableJIT;
  }

  bool breakOnLoad() const {
    return mBreakOnLoad;
  }

  QString startSessionScript()
  {
    return mStartSessionScript;
  }

  virtual void loadValuesFromWidget();
  virtual DebuggerTab* createTab();
  virtual DebuggerTab* tab();

protected:

  // Gubed
  int  mListenPort;
  bool mEnableJIT;
  bool mBreakOnLoad;
  QString mStartSessionScript;
private:
  GBSettingsWidget* m_widget;
};

#endif
