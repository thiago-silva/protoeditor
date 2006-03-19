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

  void load(const QString& name, const QString& url,
            const QString& remoteBaseDir, const QString& localBaseDir,
            const QString& defaultFile, bool matchFileInLowerCase,
            const QString& debuggerClient);

  void setName(const QString& name)
  {
    mName = name;
  }

  QString name() const
  {
    return mName;
  }


  void setUrl(const QString& url)
  {
    mUrl = url;
  }

  QString url() {
    return mUrl;
  }

  KURL effectiveURL() const
  {
    KURL url(mUrl);
    if(url.port() == 0) {
      url.setPort(80);
    }
    return url;
  }

  void setLocalBaseDir(const QString& localBaseDir)
  {
    mLocalBaseDir = localBaseDir;
  }

  QString localBaseDir() const
  {
    return mLocalBaseDir;
  }

  void setRemoteBaseDir(const QString& remoteBaseDir)
  {
    mRemoteBaseDir = remoteBaseDir;
  }

  QString remoteBaseDir() const
  {
    return mRemoteBaseDir;
  }

  void setDefaultFile(const QString& defaultFile)
  {
    mDefaultFile = defaultFile;
  }

  QString defaultFile() const
  {
    return mDefaultFile;
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
