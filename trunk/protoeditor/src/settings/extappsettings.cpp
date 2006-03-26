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

#include "extappsettings.h"

ExtAppSettings::ExtAppSettings(  )
  : KConfigSkeleton( QString::fromLatin1( "protoeditorrc" ) )
{
  setCurrentGroup( QString::fromLatin1( "ExternalApp" ) );

  KConfigSkeleton::ItemBool  *itemUseExternalApp;
  itemUseExternalApp = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "UseExternalApp" ), mUseExternalApp, false );
  addItem( itemUseExternalApp, QString::fromLatin1( "UseExternalApp" ) );
  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesApp;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Konqueror" );
    valuesApp.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Mozilla" );
    valuesApp.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Firefox" );
    valuesApp.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Opera" );
    valuesApp.append( choice );
  }
//   {
//     KConfigSkeleton::ItemEnum::Choice choice;
//     choice.name = QString::fromLatin1( "Console" );
//     valuesApp.append( choice );
//   }
  KConfigSkeleton::ItemEnum  *itemExtApp;
  itemExtApp = new KConfigSkeleton::ItemEnum( currentGroup(), QString::fromLatin1( "App" ), mExternalApp, valuesApp, EnumExtApp::Konqueror );
  addItem( itemExtApp, QString::fromLatin1( "Session" ) );

  KConfigSkeleton::ItemBool  *itemUseConsole;
  itemUseConsole = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "UseConsole" ), mUseConsole, true);
  addItem( itemUseConsole, QString::fromLatin1( "UseConsole" ) );

  KConfigSkeleton::ItemString  *itemConsole;
  itemConsole = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "Console" ), mConsole, "konsole -e");
  addItem( itemConsole, QString::fromLatin1( "Console" ) );
/*
  KConfigSkeleton::ItemString  *itemShellExec;
  itemShellExec = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "ShellExec" ), mShellExec);
  addItem( itemShellExec, QString::fromLatin1( "ShellExec" ) );*/

  readConfig();
}

ExtAppSettings::~ExtAppSettings()
{
}

