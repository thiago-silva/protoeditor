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

#ifndef PYTHONDBGPSETTINGS_H
#define PYTHONDBGPSETTINGS_H

#include "debuggersettingsinterface.h"

class PythonDBGPSettingsWidget;
class LanguageSettings;

class PythonDBGPSettings : public DebuggerSettingsInterface
{
  public:
    PythonDBGPSettings(const QString&, const QString&, const QString&, LanguageSettings*);
    ~PythonDBGPSettings();

    int listenPort() const
    {
      return mListenPort;
    }

    bool enableJIT() const
    {
      return mEnableJIT;
    }

    bool sendSuperGlobals() const {
      return mSendSuperGlobals;
    }

    bool breakOnLoad() const {
      return mBreakOnLoad;
    }

    void setPerDBGPlLibPath(const QString& path)
    {
      mPythonDBGPLibPath = path;
    }

    QString pythonDBGPLibPath() const
    {
      return mPythonDBGPLibPath;
    }

    virtual void loadValuesFromWidget();
    virtual DebuggerTab* createTab();
    virtual DebuggerTab* tab();

  protected:

    // Xdebug
    int  mListenPort;
    bool mEnableJIT;
    bool mSendSuperGlobals;
    bool mBreakOnLoad;
    QString mPythonDBGPLibPath;
  private:
    PythonDBGPSettingsWidget* m_widget;
};

#endif
