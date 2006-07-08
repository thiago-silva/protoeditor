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

#ifndef LANGUAGESETTINGSINTERFACE_H
#define LANGUAGESETTINGSINTERFACE_H

#include <kconfigskeleton.h>
#include <qmap.h>

class QString;
class DebuggerSettingsInterface;
class AbstractDebugger;

class LanguageSettingsWidget
{
  public:
    virtual ~LanguageSettingsWidget() {};

    virtual void populate() = 0;
    virtual void updateSettings() = 0;
};

class LanguageSettings : public KConfigSkeleton
{
public:
  LanguageSettings(const QString&);

  virtual         ~LanguageSettings();
  
  virtual bool    isEnabled()          const = 0;
  virtual QString languageName()       const = 0;
  virtual QString iconName()           const = 0;
  virtual QString interpreterCommand() const = 0;
  virtual QString defaultDebugger()    const = 0;

  virtual void writeConfig();

  void registerDebuggerSettings(const QString& name, DebuggerSettingsInterface* dbsettings);

  DebuggerSettingsInterface* debuggerSettings(const QString& name);

  QValueList<DebuggerSettingsInterface*> debuggerSettingsList();
  
  virtual QValueList<AbstractDebugger*> debuggers() = 0;

  virtual LanguageSettingsWidget* createSettingsWidget(QWidget*) = 0;

protected:
  QMap<QString, DebuggerSettingsInterface*>  m_debuggerSettingsMap;
};

#endif
