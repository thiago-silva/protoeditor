#ifndef KATEEDITORUI_H
#define KATEEDITORUI_H

#include "editorinterface.h"

#include <qobject.h>
#include <qptrlist.h>

namespace Kate { 
  class Document;
}
class KateDocWrapper;

class KateUInterface;

class KateEditorUI : public QObject,
                     public EditorInterface
{
  Q_OBJECT
public:
  KateEditorUI(KateUInterface* ui);
  ~KateEditorUI();

  bool openDocument(const KURL& url);

  void activateDocument(const KURL&, bool forceOpen = false);
  bool saveCurrentFile();
  bool saveCurrentFileAs(const KURL & url, const QString& encoding = QString::null);
  void gotoLineAtFile(const KURL&, int);
  KURL currentDocumentURL();
  int  currentDocumentLine();
  bool currentDocumentExistsOnDisk();

  void markActiveBreakpoint(const KURL&, int);
  void unmarkActiveBreakpoint(const KURL&, int);
  void markDisabledBreakpoint(const KURL&, int);
  void unmarkDisabledBreakpoint(const KURL&, int);
  void markExecutionPoint(const KURL&, int);
  void unmarkExecutionPoint(const KURL&);
  void markPreExecutionPoint(const KURL&, int);
  void unmarkPreExecutionPoint(const KURL&);
  bool hasBreakpointAt(const KURL& , int);
  void saveExistingFiles();

  int totalDocuments();

  void setup();

  void setTerminating();
  bool terminating();  

  int documentIndex(const KURL&);
  Kate::Document* documentFromID(uint);
public slots:

//   void slotAddWatch();
  
  void slotDocumentCreated(Kate::Document *);
  void slotDocumentDeleted(uint);
//   void slotMenuAboutToShow(); 

  void slotDocumentOpened(const KURL&);
signals:
  void sigFirstPage();
  void sigNewPage();
  void sigAddWatch(const QString&);

  void sigBreakpointUnmarked(const KURL&, int);
  void sigBreakpointMarked(const KURL&, int, bool enabled);

private:  
  Kate::Document* currentDocument();

  Kate::Document* documentFromURL(const KURL&);
  

  KateUInterface* m_ui;
  bool m_terminating;

  QMap<Kate::Document*, KateDocWrapper*> m_docMap;
};

#endif
