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

#include "breakpointlistviewitem.h"
#include "breakpointlistview.h"
#include "debuggerbreakpoint.h"
#include <kiconloader.h>

BreakpointListViewItem::BreakpointListViewItem(KListView *parent)
    : KListViewItem(parent), m_breakpoint(0)
{
  setRenameEnabled(BreakpointListView::ConditionCol, true);
  setRenameEnabled(BreakpointListView::SkipHitsCol,  true);

}

BreakpointListViewItem::BreakpointListViewItem(KListView *parent, DebuggerBreakpoint* bp)
    : KListViewItem(parent), m_breakpoint(0)
{
  setRenameEnabled(BreakpointListView::ConditionCol, true);
  setRenameEnabled(BreakpointListView::SkipHitsCol,  true);

  setBreakpoint(bp);
}

BreakpointListViewItem::~BreakpointListViewItem()
{
  delete m_breakpoint;
}

void BreakpointListViewItem::setBreakpoint(DebuggerBreakpoint* bp)
{
  //TODO, make the pixmap members of the class (we are loading everytime...)

  delete m_breakpoint;
  m_breakpoint = bp;
  showBreakpoint();
}

void BreakpointListViewItem::showBreakpoint() {

  KIconLoader *loader = KGlobal::iconLoader();
  switch(m_breakpoint->status()) {
      case DebuggerBreakpoint::ENABLED:
      setText(BreakpointListView::StatusTextCol, "Enabled");
      setPixmap(BreakpointListView::StatusIconCol, loader->loadIcon(
                  "checkboxon", KIcon::User));
      break;

      case DebuggerBreakpoint::DISABLED:
      setText(BreakpointListView::StatusTextCol, "Disabled");
      setPixmap(BreakpointListView::StatusIconCol, loader->loadIcon(
                  "checkboxoff", KIcon::User));
      break;

      case DebuggerBreakpoint::UNRESOLVED:
      default:
      setText(BreakpointListView::StatusTextCol, "Unresolved");
      setPixmap(BreakpointListView::StatusIconCol, loader->loadIcon(
                  "interrogation", KIcon::User));
      break;
  }

  setText(BreakpointListView::FileNameCol,  m_breakpoint->filePath());
  setText(BreakpointListView::LineCol,      QString::number(m_breakpoint->line()));
  setText(BreakpointListView::ConditionCol, m_breakpoint->condition());
  setText(BreakpointListView::SkipHitsCol,  QString::number(m_breakpoint->skipHits()));
  setText(BreakpointListView::HitCountCol,  QString::number(m_breakpoint->hitCount()));


}

void BreakpointListViewItem::reset() {

  m_breakpoint->setId(0);
  if(m_breakpoint->status() == DebuggerBreakpoint::UNRESOLVED) {
    m_breakpoint->setStatus(DebuggerBreakpoint::ENABLED);
  }
  showBreakpoint();
  /*
  KIconLoader *loader = KGlobal::iconLoader();

  setText(BreakpointListView::StatusTextCol, "Enabled");
  setPixmap(BreakpointListView::StatusIconCol, loader->loadIcon(
    "checkboxoff", KIcon::User));
  */
}

DebuggerBreakpoint* BreakpointListViewItem::breakpoint()
{
  return m_breakpoint;
}
