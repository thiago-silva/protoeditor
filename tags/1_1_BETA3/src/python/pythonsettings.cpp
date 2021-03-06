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

#include "pythonsettings.h"
#include "pythonsettingswidget.h"
#include "debuggerpythondbgp.h"


QString PythonSettings::lang = "Python";

PythonSettings::PythonSettings(const QString& configFile )
  : LanguageSettings( QString::fromLatin1( configFile ) )
{
  AbstractDebugger* def = new DebuggerPythonDBGP(this);
  m_debuggerList.append(def);

  setCurrentGroup( QString::fromLatin1( "Python" ) );

  KConfigSkeleton::ItemString  *itemPythonCommand;
  itemPythonCommand = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "PythonCommand" ), mPythonCommand, "python");
  addItem( itemPythonCommand, QString::fromLatin1( "PythonCommand" ) );

  KConfigSkeleton::ItemString  *itemDefaultDebugger;
  itemDefaultDebugger = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "DefaultDebugger" ), mDefaultDebugger, def->name());
  addItem( itemDefaultDebugger, QString::fromLatin1( "DefaultDebugger" ) );

  KConfigSkeleton::ItemBool  *itemEnabled;
  itemEnabled = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "Enabled" ), mEnabled, true );
  addItem(itemEnabled, QString::fromLatin1( "Enabled" ) );

  readConfig();
}

PythonSettings::~PythonSettings()
{
}


QValueList<AbstractDebugger*> PythonSettings::debuggers()
{
  return m_debuggerList;     
}

LanguageSettingsWidget* PythonSettings::createSettingsWidget(QWidget* parent)
{
  return new PythonSettingsWidget(this, parent);
}
