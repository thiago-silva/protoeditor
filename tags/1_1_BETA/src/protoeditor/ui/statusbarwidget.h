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

#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include <kstatusbar.h>
class QLabel;

class StatusBarWidget : KStatusBar
{
  Q_OBJECT
public:
  enum { LedOn, LedOff /*, LedWait */};

  StatusBarWidget(QWidget* parent, const char *name = 0);
  ~StatusBarWidget();

  void setEditorStatus(const QString&);
  void setDebugMsg(const QString&);
  void setDebuggerName(const QString&);
  void setLedState(int);

private:
  QLabel *m_lbLed;
  QLabel *m_lbDebugMsg;
  QLabel *m_lbDebugName;
  QLabel *m_lbEditorMsg;

};

#endif
