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
 
#include "sitesettings.h"

SiteSettings::SiteSettings(const QString &number)
  : KConfigSkeleton( QString::fromLatin1( "protoeditorrc" ) )
  , mParamnumber(number)
{
  setCurrentGroup( QString::fromLatin1( "Site_%1" ).arg( mParamnumber ) );

  KConfigSkeleton::ItemString  *itemName;
  itemName = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "Name" ), mName );
  addItem( itemName, QString::fromLatin1( "Name" ) );
  KConfigSkeleton::ItemString  *itemUrl;
  itemUrl = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "URL" ), mUrl );
  addItem( itemUrl, QString::fromLatin1( "URL" ) );
  
  KConfigSkeleton::ItemPath  *itemLocalBaseDir;
  itemLocalBaseDir = new KConfigSkeleton::ItemPath( currentGroup(), QString::fromLatin1( "LocalBaseDir" ), mLocalBaseDir );
  addItem( itemLocalBaseDir, QString::fromLatin1( "LocalBaseDir" ) );

  KConfigSkeleton::ItemPath  *itemRemoteBaseDir;
  itemRemoteBaseDir = new KConfigSkeleton::ItemPath( currentGroup(), QString::fromLatin1( "RemoteBaseDir" ), mRemoteBaseDir );
  addItem( itemRemoteBaseDir, QString::fromLatin1( "RemoteBaseDir" ) );

  KConfigSkeleton::ItemPath  *itemDefaultFile;
  itemDefaultFile = new KConfigSkeleton::ItemPath( currentGroup(), QString::fromLatin1( "DefaultFile" ), mDefaultFile);
  addItem( itemDefaultFile, QString::fromLatin1( "DefaultFile" ) );
  
  KConfigSkeleton::ItemBool  *itemMatchFileInLowerCase;
  itemMatchFileInLowerCase = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "MatchFileInLowerCase" ), mMatchFileInLowerCase, false );
  addItem( itemMatchFileInLowerCase, QString::fromLatin1( "MatchFileInLowerCase" ) );

  KConfigSkeleton::ItemString  *itemDebuggerClient;
  itemDebuggerClient = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "DebuggerClient" ), mDebuggerClient, "DBG" );
  addItem( itemDebuggerClient, QString::fromLatin1( "DebuggerClient" ) );

  readConfig();
}


void SiteSettings::load(const QString& name, const QString& url,/* int port,*/
     const QString& remoteBaseDir, const QString& localBaseDir,
     const QString& defaultFile, bool matchFileInLowerCase,
     const QString& debuggerClient)
{
  mName = name;
  mUrl = url;
  mRemoteBaseDir = remoteBaseDir;
  mLocalBaseDir = localBaseDir;
  mMatchFileInLowerCase = matchFileInLowerCase;
  mDebuggerClient = debuggerClient;
  mDefaultFile = defaultFile;
}

SiteSettings::~SiteSettings()
{
}

