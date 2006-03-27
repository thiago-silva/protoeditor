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
#ifndef SITESETTINGSDIALOG_H
#define SITESETTINGSDIALOG_H

#include <kdialogbase.h>
#include <kurl.h>

class KLineEdit;
class QSpinBox;
class KURLRequester;
class QComboBox;


class DirMappingWidget : public QWidget {
  Q_OBJECT
public:
  enum { LocalColumn, RemoteColumn };

  DirMappingWidget(QWidget* parent, const char* name = 0);
  ~DirMappingWidget();

  void setMappings(const QMap<QString,QString>&);
  QMap<QString,QString> mappings();

private slots:
  void slotAdd();
  void slotUpdate();
  void slotRemove();

  void slotSelectionChanged();
private:
  
  void initListView();

  void addMapping(const QString& local, const QString& remote);

  bool itemExists(const QString&);  

  QPushButton   *m_btAdd;
  QPushButton   *m_btUpdate;
  QPushButton   *m_btRemove;
  KURLRequester* m_edLocalDir;
  KLineEdit*     m_edRemoteDir;
  KListView*     m_listView;
};

class SiteSettingsDialog : public KDialogBase
{
  Q_OBJECT
public:
  SiteSettingsDialog(QWidget* parent, const char* name = 0);
  ~SiteSettingsDialog();

  QString name();
  KURL    url();
  KURL    remoteBaseDir();
  KURL    localBaseDir();
  KURL    defaultFile();
  QString debuggerClient();
  
  QMap<QString,QString> mappings();

  void populate(const QString& name, const KURL& url,
                const KURL& remoteBaseDir, const KURL& localBaseDir,
                const KURL& defaultFile, const QString& debuggerClient,
                QMap<QString,QString>&);

  void setUpdate();
protected slots:
  void slotOk();
  void slotOpenFileDialog(KURLRequester*);
private:
  DirMappingWidget *m_mappingWidget;
  KLineEdit        *m_edName;
  KLineEdit        *m_edUrl;
  KLineEdit        *m_edRemoteBaseDir;
  KURLRequester    *m_edLocalBaseDir;
  KURLRequester    *m_edDefaultFile;
  QComboBox        *m_cbDebuggerClient;
};

#endif
