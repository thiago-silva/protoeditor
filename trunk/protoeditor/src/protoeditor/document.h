#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <qobject.h>
#include <qvaluelist.h>
#include <qstring.h>

#include <ktexteditor/markinterface.h>

class QWidget;

namespace KTextEditor
{
  class View;
}

class KURL;

class Document : public QObject
{
  Q_OBJECT
public:
  Document(QWidget *parent, const char *name = 0);

  ~Document();

  KTextEditor::View* view();
  const QString& path();
  QWidget* tab();
  
  bool save();
  bool saveAs(const KURL&);
  bool open(const KURL& url);
  bool close();
  
  void gotoLine(int line);
  int currentLine();
  void markActiveBreakpoint(int line);
  void unmarkActiveBreakpoint(int line);
  void markDisabledBreakpoint(int line);
  void unmarkDisabledBreakpoint(int line);
  void markExecutionPoint(int line);
  void unmarkExecutionPoint();
  void markPreExecutionPoint(int line);
  void unmarkPreExecutionPoint();
  bool hasBreakpointAt(int line);

signals:
  void sigBreakpointUnmarked(Document*, int);
  void sigBreakpointMarked(Document*, int);
    
private slots:
  void slotMarkChanged();

private:
 
  void setupMarks();
  void addMark(int line, /*KTextEditor::MarkInterface::MarkTypes type*/ uint type);
  void removeMark(int line, /*KTextEditor::MarkInterface::MarkTypes type*/ uint type);

  bool m_terminating;
  
  //so external marks (from mark*/unmark* funcs) don't emit signals
  bool m_userMarkedGuard;

//   bool m_processingMark;
  
  QWidget* m_tab;
  KTextEditor::View* m_view;
  //we need this if the user edit the file when debugging
  //Since debuggermanager would try to unmark the execmark that
  //is no longer there (kate takes care of changing the line
  //of the mark when edited) causing to have more than 2 execMarks and, after the debug ends
  //leaving a exec mark showing
  int m_execLine;
  int m_preExecLine;

  QString m_path;
  QValueList<KTextEditor::Mark> m_breakpointMarks;
};

#endif
