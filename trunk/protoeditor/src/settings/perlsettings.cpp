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

#include "perlsettings.h"

QString PerlSettings::lang = "Perl";

PerlSettings::PerlSettings(  )
  : LanguageSettings( QString::fromLatin1( "protoeditorrc" ) )
{
  setCurrentGroup( QString::fromLatin1( "Perl" ) );

  KConfigSkeleton::ItemString  *itemPerlCommand;
  itemPerlCommand = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "PerlCommand" ), mPerlCommand, "perl");
  addItem( itemPerlCommand, QString::fromLatin1( "PerlCommand" ) );

  KConfigSkeleton::ItemString  *itemDefaultDebugger;
  itemDefaultDebugger = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "DefaultDebugger" ), mDefaultDebugger );
  addItem( itemDefaultDebugger, QString::fromLatin1( "DefaultDebugger" ) );

  KConfigSkeleton::ItemBool  *itemEnabled;
  itemEnabled = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "Enabled" ), mEnabled, false );
  addItem(itemEnabled, QString::fromLatin1( "Enabled" ) );

  readConfig();
}

PerlSettings::~PerlSettings()
{
}

