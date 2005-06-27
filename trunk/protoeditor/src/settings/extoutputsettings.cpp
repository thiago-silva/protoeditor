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

#include "extoutputsettings.h"

ExtOutputSettings::ExtOutputSettings(  )
  : KConfigSkeleton( QString::fromLatin1( "protoeditorrc" ) )
{
  setCurrentGroup( QString::fromLatin1( "ExternalOutput" ) );

  KConfigSkeleton::ItemBool  *itemUseExternalBrowser;
  itemUseExternalBrowser = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "UseExternalBrowser" ), mUseExternalBrowser, false );
  addItem( itemUseExternalBrowser, QString::fromLatin1( "UseExternalBrowser" ) );
  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesBrowser;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Konqueror" );
    valuesBrowser.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Mozilla" );
    valuesBrowser.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Firefox" );
    valuesBrowser.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Opera" );
    valuesBrowser.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemBrowser;
  itemBrowser = new KConfigSkeleton::ItemEnum( currentGroup(), QString::fromLatin1( "Browser" ), mBrowser, valuesBrowser, EnumBrowser::Konqueror );
  addItem( itemBrowser, QString::fromLatin1( "Browser" ) );

  readConfig();
}

ExtOutputSettings::~ExtOutputSettings()
{
}

