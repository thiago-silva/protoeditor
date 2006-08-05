#include "katedebuggerui.h"

#include <kate/toolviewmanager.h>
#include <kate/mainwindow.h>

#include <kiconloader.h>
#include <klocale.h>

#include "kateuinterface.h"

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

#include "protoeditor.h"
#include "datacontroller.h"
#include "executioncontroller.h"


KateDebuggerUI::KateDebuggerUI(KateUInterface* ui)
{
  m_dockList.setAutoDelete(true);

  Kate::ToolViewManager *tool_view_manager = ui->window()->toolViewManager();

  QWidget* dock;

  dock = tool_view_manager->createToolView("kate_plugin_phpdebugger_global", 
      Kate::ToolViewManager::Bottom, SmallIcon("network"), i18n("Global"));

  m_dockList.append(dock);

  m_globalVariableListView = new VariableListView(GlobalVarListID, dock);


  dock = tool_view_manager->createToolView("kate_plugin_phpdebugger_local", 
      Kate::ToolViewManager::Bottom, SmallIcon("kfm_home"), i18n("Local"));

  m_dockList.append(dock);

  m_localTab = new LocalTab(dock);

  dock = tool_view_manager->createToolView("kate_plugin_phpdebugger_watches", 
      Kate::ToolViewManager::Bottom, SmallIcon("viewmag"), i18n("Watches"));

  m_dockList.append(dock);

  m_watchTab = new WatchTab(dock);

  dock = tool_view_manager->createToolView("kate_plugin_phpdebugger_breakpoints", 
      Kate::ToolViewManager::Bottom, SmallIcon("stop"), i18n("Breakpoints"));

  m_dockList.append(dock);

  m_breakpointListView = new BreakpointListView(dock);

  dock = tool_view_manager->createToolView("kate_plugin_phpdebugger_messages", 
      Kate::ToolViewManager::Bottom, SmallIcon("info"), i18n("Messages"));

  m_dockList.append(dock);

  m_messageListView = new MessageListView(dock);

  dock = tool_view_manager->createToolView("kate_plugin_phpdebugger_output", 
      Kate::ToolViewManager::Bottom, SmallIcon("openterm"), i18n("Output"));

  m_dockList.append(dock);

  m_edOutput = new KTextEdit(dock);
  m_edOutput->setReadOnly(true);
  m_edOutput->setTextFormat(Qt::PlainText);
  m_edOutput->setPaper(QBrush(QColor("white")));

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

  connect(this, SIGNAL(sigGotoFileAndLine(const KURL&, int)),
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


KateDebuggerUI::~KateDebuggerUI()
{
}

void KateDebuggerUI::slotBreakpointMarked(const KURL& url, int line, bool enabled)
{
  m_breakpointListView->toggleBreakpoint(url, line, enabled); 
}

void KateDebuggerUI::slotBreakpointUnmarked(const KURL& url, int line)
{
  m_breakpointListView->toggleBreakpoint(url, line); 
}

void KateDebuggerUI::slotNeedChildren(int varlistID, Variable* var)
{
  m_varlistIDs.append(varlistID);
  
  switch(varlistID)
  {
    case GlobalVarListID:
      emit sigNeedChildren(
        m_localTab->comboStack()->stack()->topExecutionPoint()->id(), var);
      break;
    case LocalVarListID:
    case WatchListID:
      emit sigNeedChildren(
        m_localTab->comboStack()->selectedDebuggerExecutionPoint()->id(), var);
      break;
    default:
      //error
      break;
  }
}

#include "katedebuggerui.moc"
