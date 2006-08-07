#ifndef KATEDOCWRAPPER_H
#define KATEDOCWRAPPER_H

#include <ktexteditor/markinterface.h>

#include <kurl.h>
#include <qobject.h>
#include <qvaluelist.h>
#include <qstring.h>


namespace Kate {
  class Document;
}

class KateEditorUI;

class KateDocWrapper : public QObject
{
  Q_OBJECT
public:
  KateDocWrapper(Kate::Document*, KateEditorUI*);
  ~KateDocWrapper();

  void markActiveBreakpoint(int line);
  void unmarkActiveBreakpoint(int line);
  void markDisabledBreakpoint(int line);
  void unmarkDisabledBreakpoint(int line);
  void markExecutionPoint(int line);
  void unmarkExecutionPoint();
  void markPreExecutionPoint(int line);
  void unmarkPreExecutionPoint();
  bool hasBreakpointAt(int line);

  QString docName();
signals:
  void sigBreakpointUnmarked(const KURL&, int);
  void sigBreakpointMarked(const KURL&, int, bool enabled);
  void sigDocumentOpened(const KURL&);
private slots:
  void slotMarkChanged();
  void slotDocumentOpened();
private:
  void setupMarks();
  void addMark(int line, unsigned int type);
  void removeMark(int line, unsigned int type);

  KateEditorUI* m_editor;

  Kate::Document* m_doc;

  bool m_terminating;
  
  //so external marks (from mark*/unmark* funcs) don't emit signals
  bool m_userMarkedGuard;

//   KTextEditor::View* m_view;

  //we need this if the user edit the file when debugging
  //Since protoeditor would try to unmark the execmark that
  //is no longer there (kate takes care of changing the line
  //of the mark when edited) causing to have more than 2 execMarks and, after the debug ends
  //leaving a exec mark showing
  int m_execLine;
  int m_preExecLine;

  QValueList<KTextEditor::Mark> m_breakpointMarks;
};

#endif
