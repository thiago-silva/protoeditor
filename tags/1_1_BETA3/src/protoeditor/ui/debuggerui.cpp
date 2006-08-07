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

#include "debuggerui.h"

#include "protoeditor.h"
#include "datacontroller.h"
#include "executioncontroller.h"

#include <qlayout.h>
#include <klocale.h>


#include <ktextedit.h>

#include "variablelistview.h"
#include "localtab.h"
#include "watchtab.h"
#include "breakpointlistview.h"
#include "messagelistview.h"
#include "watchlistview.h"
#include "combostack.h"
#include "consolewidget.h"
#include "debuggerstack.h"

DebuggerUI::DebuggerUI(QWidget* parent, const char *name)
  : KTabWidget(parent, name)
{  
  setGeometry(0,0,0,height()/15);
  QWidget* globalVarTab = new QWidget(this);
  QVBoxLayout* globalVarTabLayout = new QVBoxLayout(globalVarTab, 1, 1);

  m_globalVariableListView = new VariableListView(DebuggerUI::GlobalVarListID, globalVarTab);
  globalVarTabLayout->addWidget(m_globalVariableListView);
  insertTab(globalVarTab, i18n("Global"));

  m_localTab = new LocalTab(this);
  insertTab(m_localTab, i18n("Local"));

  m_watchTab = new WatchTab(this);
  insertTab(m_watchTab, i18n("Watches"));

  QWidget* breakpointTab = new QWidget(this);
  QVBoxLayout* breakpointTabLayout = new QVBoxLayout(breakpointTab, 1, 1);

  m_breakpointListView = new BreakpointListView(breakpointTab);
  breakpointTabLayout->addWidget(m_breakpointListView);
  insertTab(breakpointTab, i18n("Breakpoints"));

  QWidget* messageTab = new QWidget(this);
  QVBoxLayout* logTabLayout = new QVBoxLayout(messageTab, 1, 1);

  m_messageListView = new MessageListView(messageTab);
  logTabLayout->addWidget(m_messageListView);
  insertTab(messageTab, i18n("Messages"));

  QWidget* outputTab = new QWidget(this);
  QVBoxLayout* outputTabLayout = new QVBoxLayout(outputTab, 1, 1);

  m_edOutput = new KTextEdit(outputTab);
  outputTabLayout->addWidget(m_edOutput);
  m_edOutput->setReadOnly(true);
  m_edOutput->setTextFormat(Qt::PlainText);
  m_edOutput->setPaper(QBrush(QColor("white")));
  insertTab(outputTab, i18n("Output"));

//   m_console = new ConsoleWidget(this);
//   insertTab(m_console, i18n("Console"));


  connect(m_globalVariableListView, SIGNAL(sigVarModified(Variable*)),
    this, SIGNAL(sigGlobalVarModified(Variable*)));

  connect(m_globalVariableListView, SIGNAL(sigNeedChildren(int, Variable*)),
    this, SLOT(slotNeedChildren( int, Variable* )));

  connect(m_localTab->localVarList(), SIGNAL(sigVarModified(Variable*)),
    this, SIGNAL(sigLocalVarModified(Variable*)));

  connect(m_localTab->localVarList(), SIGNAL(sigNeedChildren(int, Variable*)),
    this, SLOT(slotNeedChildren( int, Variable* )));

  connect(m_localTab->comboStack(), 
      SIGNAL(changed(DebuggerExecutionPoint*, DebuggerExecutionPoint*)),
      this, SIGNAL(sigStackchanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)));

  connect(m_watchTab, SIGNAL(sigWatchAdded(const QString&)),
    this, SIGNAL(sigWatchAdded(const QString&)));

  connect(m_watchTab->watchListView(), SIGNAL(sigVarModified(Variable*)),
    this, SIGNAL(sigWatchModified(Variable*)));

  connect(m_watchTab->watchListView(), SIGNAL(sigWatchRemoved(Variable*)),
    this, SIGNAL(sigWatchRemoved(Variable*)));

  connect(m_watchTab->watchListView(), SIGNAL(sigNeedChildren(int, Variable*)),
    this, SLOT(slotNeedChildren( int, Variable* )));

  connect(m_breakpointListView, SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
    this, SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)));

  connect(m_breakpointListView, SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)),
    this, SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)));

  connect(m_breakpointListView, SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)),
    this, SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)));

  connect(m_breakpointListView, SIGNAL(sigDoubleClick(const KURL&, int)),
    this, SIGNAL(sigGotoFileAndLine(const KURL&, int)));

  connect(m_messageListView, SIGNAL(sigDoubleClick(const KURL&, int)),
    this, SIGNAL(sigGotoFileAndLine(const KURL&, int)));

// //   connect(m_console, SIGNAL(sigExecuteCmd(const QString&)),
// //     this, SIGNAL(sigExecuteCmd(const QString&)));

  connect(this, SIGNAL(sigGotoFileAndLine( const KURL&, int )),
          Protoeditor::self(), SLOT(slotGotoLineAtFile(const KURL&, int)));

  connect(this, SIGNAL(sigGlobalVarModified(Variable*)),
          Protoeditor::self()->dataController(), SLOT(slotGlobalVarModified(Variable*)));

  connect(this, SIGNAL(sigLocalVarModified(Variable*)),
          Protoeditor::self()->dataController(), SLOT(slotLocalVarModified(Variable*)));

  connect(this, SIGNAL(sigWatchAdded(const QString&)),
          Protoeditor::self()->dataController(), SLOT(slotWatchAdded(const QString&)));

  connect(this, SIGNAL(sigWatchRemoved(Variable*)),
          Protoeditor::self()->dataController(), SLOT(slotWatchRemoved(Variable*)));

  connect(this, SIGNAL(sigBreakpointCreated(DebuggerBreakpoint*)),
          Protoeditor::self()->dataController(), SLOT(slotBreakpointCreated(DebuggerBreakpoint*)));

  connect(this, SIGNAL(sigBreakpointChanged(DebuggerBreakpoint*)),
          Protoeditor::self()->dataController(), SLOT(slotBreakpointChanged(DebuggerBreakpoint*)));

  connect(this, SIGNAL(sigBreakpointRemoved(DebuggerBreakpoint*)),
          Protoeditor::self()->dataController(), SLOT(slotBreakpointRemoved(DebuggerBreakpoint*)));

  connect(this,SIGNAL(sigStackchanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)), 
          Protoeditor::self()->dataController(), SLOT(slotStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)));  

  connect(this, SIGNAL(sigStackchanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)),
          Protoeditor::self()->dataController(), SLOT(slotStackChanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*)));

  connect(this, SIGNAL(sigWatchModified(Variable*)),
          Protoeditor::self()->dataController(), SLOT(slotLocalVarModified(Variable*)));

  connect(this, SIGNAL(sigExecuteCmd(const QString&)),
          Protoeditor::self()->executionController(), SLOT(slotExecuteCmd(const QString&)));

  connect(this, SIGNAL(sigNeedChildren(int, Variable*)),
          Protoeditor::self()->executionController(), SLOT(slotNeedChildren(int, Variable*)));

}

DebuggerUI::~DebuggerUI()
{
}

void DebuggerUI::slotBreakpointMarked(const KURL& url, int line, bool enabled)
{
  m_breakpointListView->toggleBreakpoint(url, line, enabled); 
}

void DebuggerUI::slotBreakpointUnmarked(const KURL& url, int line)
{
  m_breakpointListView->toggleBreakpoint(url, line); 
}

void DebuggerUI::slotNeedChildren(int varlistID, Variable* var)
{
  m_varlistIDs.append(varlistID);
  
  switch(varlistID)
  {
    case DebuggerInterface::GlobalVarListID:
      emit sigNeedChildren(
        m_localTab->comboStack()->stack()->topExecutionPoint()->id(), var);
      break;
    case DebuggerInterface::LocalVarListID:
    case DebuggerInterface::WatchListID:
      emit sigNeedChildren(
        m_localTab->comboStack()->selectedDebuggerExecutionPoint()->id(), var);
      break;
    default:
      //error
      break;
  }
}
#include "debuggerui.moc"
