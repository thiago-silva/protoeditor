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
 
#ifndef PROTOEDITORSETTINGS_H
#define PROTOEDITORSETTINGS_H

#include <qobject.h>
#include <kconfigskeleton.h>
#include <qstring.h>

#include <qmap.h>
#include <qvaluelist.h>

class DebuggerSettingsInterface;
class DebuggerSettings;
class SiteSettings;
class PHPSettings;
class ExtOutputSettings;

class ProtoeditorSettings  : public QObject, KConfigSkeleton  {
  Q_OBJECT
public:
  ~ProtoeditorSettings();

  static ProtoeditorSettings* self();

  void registerDebuggerSettings(DebuggerSettingsInterface*, const QString& name);

  DebuggerSettingsInterface*             debuggerSettings(const QString& name);
  QValueList<DebuggerSettingsInterface*> debuggerSettingsList();
  QString                                currentSiteName();
  SiteSettings*                          siteSettings(const QString& name);
  SiteSettings*                          currentSiteSettings();
  QValueList<SiteSettings*>              siteSettingsList();
  PHPSettings*                           phpSettings();
  ExtOutputSettings*                     extOutputSettings();

  void clearSites();
  void addSite(int number, const QString& name, const QString& url,
               const QString& remoteBaseDir, const QString& localBaseDir,
              const QString& defaultFile, const QString& debuggerClient);

  void writeConfig(bool silent = false);

signals:
  void sigSettingsChanged();

public slots:
  void slotCurrentSiteChanged(const QString&);
private:
  ProtoeditorSettings();

  void loadSites();

  void writeDebuggersConf();
  void writeSiteConf();

  static ProtoeditorSettings *m_self;

  PHPSettings              *m_phpSettings;
  ExtOutputSettings        *m_extOutputSettings;

  QString                   m_currentSiteName;

  QMap<QString, DebuggerSettingsInterface*>  m_debuggerSettingsMap;
  QMap<QString, SiteSettings*>               m_siteSettingsMap;
};

#endif
