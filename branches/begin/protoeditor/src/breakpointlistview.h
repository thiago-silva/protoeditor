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

#ifndef BREAKPOINTLISTVIEW_H
#define BREAKPOINTLISTVIEW_H

#include <klistview.h>
//#include <qmap.h>
#include <qptrlist.h>


class DebuggerBreakpoint;
class BreakpointListViewItem;

class BreakpointListView : public KListView
{
Q_OBJECT
public:
  enum { StatusIconCol = 0, StatusTextCol, FileNameCol
       , LineCol, ConditionCol, SkipHitsCol, HitCountCol, };

  BreakpointListView(QWidget *parent = 0, const char *name = 0);
  ~BreakpointListView();

  void updateBreakpoint(DebuggerBreakpoint*);

  void resetBreakpointItems();

  QPtrList<DebuggerBreakpoint> breakpoints();

signals:
  //this signals are sent after TextEditor processed the Marks
  void sigBreakpointCreated(DebuggerBreakpoint*);
  void sigBreakpointChanged(DebuggerBreakpoint*);
  void sigBreakpointRemoved(DebuggerBreakpoint*);

  //Breakpoint was deleted here, not from TextEditor Mark
  //void sigBreakpointDeleted(DebuggerBreakpoint*);

public slots:
  //connection with KTextEditor
  void slotBreakpointMarked(QString, int);
  void slotBreakpointUnmarked(QString, int);

private slots:
  void slotCicked(QListViewItem*, const QPoint&, int);
  void slotDoubleClick(QListViewItem *, const QPoint &, int );
  void slotItemRenamed(QListViewItem *, int col, const QString& );

protected:
  virtual void keyPressEvent(QKeyEvent* e);

private:
  //void addBreakpoint(DebuggerBreakpoint*);

  BreakpointListViewItem*  findBreakpoint(DebuggerBreakpoint*);
  BreakpointListViewItem*  findBreakpoint(QString, int);
};

#endif
