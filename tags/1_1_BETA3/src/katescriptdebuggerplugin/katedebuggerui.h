#ifndef KATEDEBUGGERUI_H
#define KATEDEBUGGERUI_H

#include <qobject.h>
#include <qptrlist.h>
#include "debuggerinterface.h"

class KateUInterface;

class VariableListView;
class LocalTab;
class WatchTab;
class BreakpointListView;
class MessageListView;
class KTextEdit;

class KateDebuggerUI : public QObject,
                       public DebuggerInterface
{
  Q_OBJECT
public:
  KateDebuggerUI(KateUInterface* ui);
  ~KateDebuggerUI();

signals:
  //All variableslistview
  void sigNeedChildren(int, Variable*);

  //Global VariableListView
  void sigGlobalVarModified(Variable*);

  //Local VariableListView
  void sigLocalVarModified(Variable*);

  //ComboStack
  void sigStackchanged(DebuggerExecutionPoint*, DebuggerExecutionPoint*);

  //Watch
  void sigWatchAdded(const QString&);
  void sigWatchModified(Variable*);
  void sigWatchRemoved(Variable*);

  //BreakpointListView
  void sigBreakpointCreated(DebuggerBreakpoint*);
  void sigBreakpointChanged(DebuggerBreakpoint*);
  void sigBreakpointRemoved(DebuggerBreakpoint*);

  //Console
  void sigExecuteCmd(const QString&);

  void sigGotoFileAndLine(const KURL&, int);

public slots:
  //BreakpointListView
  void slotBreakpointMarked(const KURL&, int, bool);
  void slotBreakpointUnmarked(const KURL&, int);

  void slotNeedChildren(int, Variable*);
private:
  QPtrList<QWidget> m_dockList;
};

#endif
