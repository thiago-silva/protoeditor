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
class QLabel;
class QSpinBox;

class SiteSettingsDialog : public KDialogBase
{
  Q_OBJECT
public:
  SiteSettingsDialog(QWidget* parent, const char* name = 0);
  ~SiteSettingsDialog();

  QString name();
  QString host();
  int            port();
  QString remoteBaseDir();
  QString localBaseDir();

  void populate(const QString& name, const QString& host, int port,
                const QString& remoteBaseDir, const QString& localBaseDir);

  void setUpdate();
protected slots:
  void slotOk();

private:
  KLineEdit *m_edName;
  KLineEdit *m_edHost;
  QSpinBox  *m_spPort;
  KLineEdit *m_edRemoteBaseDir;
  KLineEdit *m_edLocalBaseDir;


  QLabel* m_lbName;
  QLabel* m_lbHost;
  QLabel* m_lbPort;
  QLabel* m_lbRemoteBaseDir;
  QLabel* m_lbLocalBaseDir;
};

#endif
