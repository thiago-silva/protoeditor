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

#ifndef DBGPROFILEDIALOG_H
#define DBGPROFILEDIALOG_H

#include <kdialog.h>

class QToolButton;

class DBGProfileListView;
class DBGProfileData;

class DBGProfileDialog : public KDialog
{
  Q_OBJECT
public:
  DBGProfileDialog(QWidget* parent = 0, const char* name = 0);
  ~DBGProfileDialog();

  void clear();
  void addData(DBGProfileData*);

  DBGProfileListView* listview();
  
signals:
  void sigClose();

protected:
  virtual void closeEvent(QCloseEvent * e);

private slots:
  void slotModule();
  void slotContext();
  void slotDetail();
  
private:
  void addToList(DBGProfileData* data);

  QToolButton* m_btModule;
  QToolButton* m_btContext;
  QToolButton* m_btDetail;
  DBGProfileListView* m_listView;
};

#endif
