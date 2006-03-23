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

#ifndef SITESETTINGS_H
#define SITESETTINGS_H

#include <kconfigskeleton.h>

#include <kurl.h>

class SiteSettings : public KConfigSkeleton
{
public:
  SiteSettings( const QString &number );
  ~SiteSettings();

  void load(const QString& name, const KURL& url,
            const KURL& remoteBaseDir, const KURL& localBaseDir,
            const KURL& defaultFile, bool matchFileInLowerCase,
            const QString& debuggerClient);

  void setName(const QString& name)
  {
    mName = name;
  }

  QString name() const
  {
    return mName;
  }


  void setUrl(const KURL& url)
  {
    mUrl = url.pathOrURL();
  }

  KURL url() {
    return KURL::fromPathOrURL(mUrl);
  }

  KURL effectiveURL() const
  {
    KURL url = KURL::fromPathOrURL(mUrl);
    if(url.port() == 0) {
      url.setPort(80);
    }
    return url;
  }

  void setLocalBaseDir(const KURL& localBaseDir)
  {
    mLocalBaseDir = localBaseDir.pathOrURL();
  }

  KURL localBaseDir() const
  {
    return KURL::fromPathOrURL(mLocalBaseDir);
  }

  void setRemoteBaseDir(const KURL& remoteBaseDir)
  {
    mRemoteBaseDir = remoteBaseDir.pathOrURL();
  }

  KURL remoteBaseDir() const
  {
    return KURL::fromPathOrURL(mRemoteBaseDir);
  }

  void setDefaultFile(const KURL& defaultFile)
  {
    mDefaultFile = defaultFile.pathOrURL();
  }

  KURL defaultFile() const
  {
    return KURL::fromPathOrURL(mDefaultFile);
  }

  void setMatchFileInLowerCase(bool value)
  {
    mMatchFileInLowerCase = value;
  }

  bool matchFileInLowerCase() const
  {
    return mMatchFileInLowerCase;
  }

  void setDebuggerClient(const QString& name)
  {
    mDebuggerClient = name;
  }

  QString debuggerClient() const
  {
    return mDebuggerClient;
  }
 
 
  KURL mapRequestURLFor(const QString& filePath)
  {    
    QString urlPath = filePath;
    urlPath = urlPath.remove(0, mLocalBaseDir.length());

    KURL url = effectiveURL();
    if(!urlPath.startsWith("/"))
    {
      urlPath.prepend('/');
    }
    url.setPath(urlPath);
    return url;
  }

  QString mapRemoteToLocal(const QString& filePath) 
  {
    QString intersection = KURL::fromPathOrURL(filePath).path();
    intersection = KURL::fromPathOrURL(mLocalBaseDir).path() + intersection.remove(0, mRemoteBaseDir.length());

    return intersection;// + filePath.section('/', -1);
  }

  QString mapLocalToRemote(const QString& filePath)
  {
    QString intersection = KURL::fromPathOrURL(filePath).path();
    intersection = KURL::fromPathOrURL(mRemoteBaseDir).path() + intersection.remove(0, mLocalBaseDir.length()) ;

    return intersection;// + filePath.section('/', -1);
  }

protected:
  
  QString mParamnumber;

  // Site_$(number)
  QString mName;
  QString mUrl;
  QString mRemoteBaseDir;
  QString mLocalBaseDir;
  QString mDefaultFile;
  bool mMatchFileInLowerCase;
  QString mDebuggerClient;

private:
};

#endif
