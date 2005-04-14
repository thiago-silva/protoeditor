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
#ifndef SITESETTINGSDIALOG_H
#define SITESETTINGSDIALOG_H

#include <kdialogbase.h>

class KLineEdit;
class QSpinBox;
class KURLRequester;

class SiteSettingsDialog : public KDialogBase
{
  Q_OBJECT
public:
  SiteSettingsDialog(QWidget* parent, const char* name = 0);
  ~SiteSettingsDialog();

  QString name();
  QString url();
//   int     port();
  QString remoteBaseDir();
  QString localBaseDir();
  QString defaultFile();

  void populate(const QString& name, const QString& url, /*int port,*/
                const QString& remoteBaseDir, const QString& localBaseDir,
                const QString& defaultFile);

  void setUpdate();
protected slots:
  void slotOk();

private:
  KLineEdit     *m_edName;
  KLineEdit     *m_edUrl;
//   QSpinBox      *m_spPort;
  KLineEdit     *m_edRemoteBaseDir;
  KURLRequester *m_edLocalBaseDir;
  KURLRequester *m_edDefaultFile;
};

#endif
