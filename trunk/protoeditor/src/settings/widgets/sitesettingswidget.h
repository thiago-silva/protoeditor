/***************************************************************************
 *   Copyright (C) 2004 by Thiago Silva                                    *
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
#ifndef SITESETTINGSWIDGET_H
#define SITESETTINGSWIDGET_H

#include <qwidget.h>

class KPushButton;
class QListBox;
class QListBoxItem;
class SiteSettings;

class SiteSettingsWidget : public QWidget {
  Q_OBJECT
public:
  SiteSettingsWidget(QWidget *parent = 0, const char *name = 0);
  ~SiteSettingsWidget();

  KPushButton   *m_btAdd;
  KPushButton   *m_btModify;
  KPushButton   *m_btRemove;

  QListBox      *m_sitesListBox;

  void populate();
  void updateSettings();
private slots:
  void slotAdd();
  void slotModify();
  void slotRemove();
  void slotListChanged(int);
  void slotListDoubleClicked(QListBoxItem*);

private:
  void addSite(const QString& name, const QString& url, /*int port,*/
               const QString& remoteBaseDir, const QString& localBaseDir,
               const QString& defaultFile);

  void modifySite(const QString& name, const QString& url, /*int port,*/
                  const QString& remoteBaseDir, const QString& localBaseDir,
                      const QString& defaultFile);

  class Site {
  public:
    Site() /*: port(0)*/ {};
    Site(const QString& n, const QString& h, /*int p,*/
         const QString& r, const QString& l, const QString& d)
        : name(n), url(h), /*port(p),*/ remoteBaseDir(r), localBaseDir(l),
          defaultFile(d){}
    ~Site() {}

    QString name;
    QString url;
//     int     port;
    QString remoteBaseDir;
    QString localBaseDir;
    QString defaultFile;
  };

  QMap<QString, Site> m_siteMap;
};

#endif
