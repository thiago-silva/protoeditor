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

#ifndef BREAKPOINTLISTVIEW_H
#define BREAKPOINTLISTVIEW_H

#include <klistview.h>
#include <qvaluelist.h>


class DebuggerBreakpoint;
class BreakpointListViewItem;

class BreakpointListView : public KListView
{
Q_OBJECT
public:
  enum { StatusIconCol = 0, StatusTextCol, FileNameCol, FilePathCol
       , LineCol, ConditionCol, SkipHitsCol, HitCountCol, };

  BreakpointListView(QWidget *parent = 0, const char *name = 0);
  ~BreakpointListView();

  void updateBreakpoint(DebuggerBreakpoint*);

  void toggleBreakpoint(const QString&, int, bool enabled = true);

  void resetBreakpointItems();

  QValueList<DebuggerBreakpoint*> breakpoints();
  QValueList<DebuggerBreakpoint*> breakpointsFrom(const QString&);

signals:
  //this signals are sent after TextEditor processed the Marks
  void sigBreakpointCreated(DebuggerBreakpoint*);
  void sigBreakpointChanged(DebuggerBreakpoint*);
  void sigBreakpointRemoved(DebuggerBreakpoint*);

  void sigDoubleClick(const QString&, int);

  //Breakpoint was deleted here, not from TextEditor Mark
  //void sigBreakpointDeleted(DebuggerBreakpoint*);

public slots:
  //connection with KTextEditor
  void slotBreakpointMarked(const QString&, int, bool);
  void slotBreakpointUnmarked(const QString&, int);

private slots:
  void slotCicked(QListViewItem*, const QPoint&, int);
  void slotDoubleClick(QListViewItem *, const QPoint &, int );
  void slotItemRenamed(QListViewItem *, int col, const QString& );
  void slotContextMenuRequested(QListViewItem *, const QPoint &, int);

protected:
  virtual void keyPressEvent(QKeyEvent* e);

private:
  void addBreakpoint(const QString&, int, bool);
  void removeBreakpoint(DebuggerBreakpoint*);

  BreakpointListViewItem*  findBreakpoint(DebuggerBreakpoint*);
  BreakpointListViewItem*  findBreakpoint(QString, int);
};

#endif
