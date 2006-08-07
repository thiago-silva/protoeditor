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
 
#include "sitesettings.h"

SiteSettings::SiteSettings(const QString& configFile, const QString &number)
  : KConfigSkeleton( QString::fromLatin1( configFile ) )
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
  
//   KConfigSkeleton::ItemBool  *itemMatchFileInLowerCase;
//   itemMatchFileInLowerCase = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "MatchFileInLowerCase" ), mMatchFileInLowerCase, false );
//   addItem( itemMatchFileInLowerCase, QString::fromLatin1( "MatchFileInLowerCase" ) );

  KConfigSkeleton::ItemString  *itemDebuggerName;
  itemDebuggerName = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "DebuggerName" ), mDebuggerName );
  addItem( itemDebuggerName, QString::fromLatin1( "DebuggerName" ) );

  readConfig();
}

SiteSettings::~SiteSettings()
{
}


void SiteSettings::load(const QString& name, const KURL& url,
     const KURL& remoteBaseDir, const KURL& localBaseDir,
     const KURL& defaultFile, /*bool matchFileInLowerCase,*/
     const QString& debuggerNme, const QMap<QString,QString>& maps)
{
  mName = name;
  mUrl = url.url();
  mRemoteBaseDir = remoteBaseDir.pathOrURL();
  mLocalBaseDir = localBaseDir.pathOrURL();
//   mMatchFileInLowerCase = matchFileInLowerCase;
  mDebuggerName = debuggerNme;
  mDefaultFile = defaultFile.pathOrURL();
  mMappings = maps;
}

void SiteSettings::setName(const QString& name)
{
  mName = name;
}

QString SiteSettings::name() const
{
  return mName;
}

void SiteSettings::setUrl(const KURL& url)
{
  mUrl = url.pathOrURL();
}

KURL SiteSettings::url() {
  return KURL::fromPathOrURL(mUrl);
}

KURL SiteSettings::effectiveURL() const
{
  KURL url = KURL::fromPathOrURL(mUrl);
  if(url.port() == 0) {
    url.setPort(80);
  }
  return url;
}

void SiteSettings::setLocalBaseDir(const KURL& localBaseDir)
{
  mLocalBaseDir = localBaseDir.pathOrURL();
}

KURL SiteSettings::localBaseDir() const
{
  return KURL::fromPathOrURL(mLocalBaseDir);
}

void SiteSettings::setRemoteBaseDir(const KURL& remoteBaseDir)
{
  mRemoteBaseDir = remoteBaseDir.pathOrURL();
}

KURL SiteSettings::remoteBaseDir() const
{
  return KURL::fromPathOrURL(mRemoteBaseDir);
}

void SiteSettings::setDefaultFile(const KURL& defaultFile)
{
  mDefaultFile = defaultFile.pathOrURL();
}

KURL SiteSettings::defaultFile() const
{
  return KURL::fromPathOrURL(mDefaultFile);
}

// void SiteSettings::setMatchFileInLowerCase(bool value)
// {
//   mMatchFileInLowerCase = value;
// }

// bool SiteSettings::matchFileInLowerCase() const
// {
//   return mMatchFileInLowerCase;
// }

void SiteSettings::setDebuggerName(const QString& name)
{
  mDebuggerName = name;
}

QString SiteSettings::debuggerName() const
{
  return mDebuggerName;
}


KURL SiteSettings::mapRequestURLFor(const QString& filePath)
{    
  QString urlPath = filePath;
  urlPath = urlPath.remove(0, mLocalBaseDir.length());

  KURL url = effectiveURL();
  if(!urlPath.startsWith("/"))
  {
    urlPath.prepend('/');
  }
  url.setPath(url.path() + urlPath);
  return url;
}

QString SiteSettings::mapRemoteToLocal(const QString& filePath) 
{
  QString intersection = KURL::fromPathOrURL(filePath).path();
  QString localDir;
  QString remoteDir;

  bool usingMappings = false;

  if(filePath.find(mRemoteBaseDir) == -1)
  {
    //try mappings
    QMap<QString, QString>::iterator it;
    for(it = mMappings.begin(); it != mMappings.end(); ++it) 
    {
      if(filePath.find(it.data()) != -1)
      {
        localDir = it.key();
        remoteDir = it.data();
        usingMappings = true;
        break;
      }
    }    
  }

  //using base dirs
  if(!usingMappings)
  {
    localDir = mLocalBaseDir;
    remoteDir = mRemoteBaseDir;
  }

  intersection = KURL::fromPathOrURL(localDir).path() + intersection.remove(0, remoteDir.length());  
  return intersection;
}

QString SiteSettings::mapLocalToRemote(const QString& filePath)
{
  QString intersection = KURL::fromPathOrURL(filePath).path();
  QString localDir;
  QString remoteDir;

  bool usingMappings = false;

  if(filePath.find(mLocalBaseDir) == -1)
  {
    //try mappings
    QMap<QString, QString>::iterator it;
    for(it = mMappings.begin(); it != mMappings.end(); ++it) 
    {
      if(filePath.find(it.key()) != -1)
      {
        localDir = it.key();
        remoteDir = it.data();
        usingMappings = true;
        break;
      }
    }    
  }

  //using base dirs
  if(!usingMappings)
  {
    localDir = mLocalBaseDir;
    remoteDir = mRemoteBaseDir;
  }

  intersection = KURL::fromPathOrURL(remoteDir).path() + intersection.remove(0, localDir.length());
  return intersection;
}

void SiteSettings::setMappings(const QMap<QString,QString>& mappings)
{
  mMappings = mappings;
}

QMap<QString,QString> SiteSettings::mappings()
{
  return mMappings;
}

void SiteSettings::remove()
{
  config()->deleteGroup(currentGroup());
}


void SiteSettings::usrReadConfig()
{
  int c = 0;
  QString tmpl = "mapping_";
  QString key;
  QString val;
  while(true)
  {
    key = tmpl + QString::number(c++);
    if(config()->hasKey(key))
    {
      val = config()->readEntry(key);
      mMappings[val.section(':', 0,0)] = val.section(':', -1);
    }
    else
    {
      break;
    }
  }  
}

void SiteSettings::usrWriteConfig()
{
  QMap<QString, QString>::iterator it;
  int c = 0;
  QString key = "mapping_";
  for(it = mMappings.begin(); it != mMappings.end(); ++it, ++c) 
  {
    config()->writeEntry(key + QString::number(c), it.key() + ":" + it.data());
  }
}
