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
            const KURL& defaultFile, /*bool matchFileInLowerCase,*/
            const QString& debuggerNme, const QMap<QString,QString>&);

  void setName(const QString& name);
  QString name() const;
  void setUrl(const KURL& url);
  KURL url();

  KURL effectiveURL() const;
  void setLocalBaseDir(const KURL& localBaseDir);
  KURL localBaseDir() const;
  void setRemoteBaseDir(const KURL& remoteBaseDir);
  KURL remoteBaseDir() const;
  void setDefaultFile(const KURL& defaultFile);
  KURL defaultFile() const;
//   void setMatchFileInLowerCase(bool value);
//   bool matchFileInLowerCase() const;
  void setDebuggerName(const QString& name);
  QString debuggerName() const;

  void setMappings(const QMap<QString,QString>&);
  QMap<QString,QString> mappings();


  KURL mapRequestURLFor(const QString& filePath);
  QString mapRemoteToLocal(const QString& filePath);
  QString mapLocalToRemote(const QString& filePath);


  void remove();

  
protected:
  virtual void usrWriteConfig();
  virtual void usrReadConfig();

  QString mParamnumber;

  // Site_$(number)
  QString mName;
  QString mUrl;
  QString mRemoteBaseDir;
  QString mLocalBaseDir;
  QString mDefaultFile;
  QString mDebuggerName;
  QMap<QString,QString> mMappings;
private:
};

#endif
