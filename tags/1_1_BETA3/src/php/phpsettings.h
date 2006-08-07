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
#ifndef PHPSETTINGS_H
#define PHPSETTINGS_H

#include "languagesettings.h"
#include "phpsettingswidget.h"

#include "debuggerdbg.h"
#include "debuggerxd.h"
#include "debuggergb.h"

class PHPSettings : public LanguageSettings
{
  public:
    static QString lang;

    PHPSettings(const QString&);
    ~PHPSettings();

    virtual QString languageName() const
    {
      return PHPSettings::lang;
    }

    virtual QString iconName()  const
    {
      return QString("php");
    }

    void setEnabled(bool value)
    {
      mEnabled = value;
    }

    virtual bool isEnabled() const
    {
      return mEnabled;
    }

    void setDefaultDebugger(QString name)
    {
      mDefaultDebugger = name;
    }

    virtual QString defaultDebugger() const
    {
      return mDefaultDebugger;
    }

    void setInterpreterCommand(const QString& cmd) {
      mPHPCommand = cmd;
    }

    virtual QString interpreterCommand() const {
      return mPHPCommand ;
    }

    virtual QValueList<AbstractDebugger*> debuggers()
    {
      return m_debuggerList;
    }

    virtual LanguageSettingsWidget* createSettingsWidget(QWidget* parent)
    {
      return new PHPSettingsWidget(this, parent);
    }

  protected:

    bool    mEnabled;

    QString mDefaultDebugger;
    QString mPHPCommand;

    QValueList<AbstractDebugger*> m_debuggerList;
};

#endif
