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

#ifndef DBGSETTINGS_H
#define DBGSETTINGS_H

#include "debuggersettingsinterface.h"

class DBGSettingsWidget;

class DBGSettings : public DebuggerSettingsInterface
{
  public:
    DBGSettings(const QString&, const QString& name, const QString& label, LanguageSettings*);
    ~DBGSettings();

    bool breakOnLoad() const
    {
      return mBreakOnLoad;
    }

    bool sendErrors() const
    {
      return mSendErrors;
    }

    bool sendLogs() const
    {
      return mSendLogs;
    }

    bool sendOutput() const
    {
      return mSendOutput;
    }

    bool sendDetailedOutput() const
    {
      return mSendDetailedOutput;
    }

    int listenPort() const
    {
      return mListenPort;
    }

    bool enableJIT() const
    {
      return mEnableJIT;
    }

    virtual QString name() {
      return m_name;
    }

    virtual void loadValuesFromWidget();
    virtual DebuggerTab* createTab();
    virtual DebuggerTab* tab();
  protected:

    // DBG
    bool mBreakOnLoad;
    bool mSendErrors;
    bool mSendLogs;
    bool mSendOutput;
    bool mSendDetailedOutput;
    int  mListenPort;
    bool mEnableJIT;

    QString m_name;
private:
  DBGSettingsWidget* m_widget;
};

#endif
