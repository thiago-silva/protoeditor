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
 
#ifndef PROTOEDITORSETTINGS_H
#define PROTOEDITORSETTINGS_H

#include <qobject.h>
#include <kconfigskeleton.h>
#include <qstring.h>

#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>

class DebuggerSettingsInterface;
class DebuggerSettings;
class SiteSettings;
class LanguageSettings;
class ExtAppSettings;
class KURL;

class ProtoeditorSettings  : public QObject, KConfigSkeleton  {
  Q_OBJECT
public:

  static QString LocalSiteName;

  ~ProtoeditorSettings();

  static void dispose();
  static ProtoeditorSettings* self();
  
  void                                   setArgumentsHistory(const QStringList&);
  QStringList                            argumentsHistory();

  void                                   registerLanguage(LanguageSettings*);

  LanguageSettings*                      languageSettings(const QString&);
  QValueList<LanguageSettings*>          languageSettingsList();

  QStringList                            supportedLanguages();

  QString                                currentSiteName();
  SiteSettings*                          siteSettings(const QString& name);
  SiteSettings*                          currentSiteSettings();
  QValueList<SiteSettings*>              siteSettingsList();
  ExtAppSettings*                        extAppSettings();  

  
  void clearSites();
  void addSite(int number, const QString& name, const KURL& url,
               const KURL& remoteBaseDir, const KURL& localBaseDir,
               const KURL& defaultFile, const QString& debuggerClient, 
               const QMap<QString,QString>&);

  void writeConfig(bool silent = false);

signals:
  void sigSettingsChanged();

public slots:
  void slotCurrentSiteChanged(const QString&);
private:
  ProtoeditorSettings();

  void loadSites();

  void writeSiteConf();

  static ProtoeditorSettings *m_self;

  ExtAppSettings           *m_extApptSettings;

  QString                   m_currentSiteName;

  QStringList               m_argumentsHistory;

  QMap<QString, LanguageSettings*>           m_langSettingsMap;  
  QMap<QString, SiteSettings*>               m_siteSettingsMap;
};

#endif
