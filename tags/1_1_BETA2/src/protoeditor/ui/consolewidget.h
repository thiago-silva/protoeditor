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

#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <qwidget.h>

class KTextEdit;
class KHistoryCombo;

class ConsoleWidget : public QWidget 
{
  Q_OBJECT
public:
  ConsoleWidget(QWidget* parent, const char* name = 0);
  ~ConsoleWidget();

  void appendDebuggerText(const QString&);
  void appendUserText(const QString&);

signals:
  void sigExecuteCmd(const QString&);

private slots:
  void slotUserCmd();

private:
  KTextEdit     *m_edit;
  KHistoryCombo *m_cbCommand;
};

#endif
