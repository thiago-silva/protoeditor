#ifndef EDITORINTERFACE_H
#define EDITORINTERFACE_H

#include <qstring.h>

class KURL;

class EditorInterface
{
public:
  virtual ~EditorInterface();

  virtual void activateDocument(const KURL&, bool forceOpen = false) = 0;
  virtual bool saveCurrentFile() = 0;
  virtual bool saveCurrentFileAs(const KURL & url, const QString& encoding = QString::null) = 0;
  virtual void gotoLineAtFile(const KURL&, int) = 0;
  virtual KURL currentDocumentURL() = 0;
  virtual int  currentDocumentLine() = 0;
  virtual bool currentDocumentExistsOnDisk() = 0;

  virtual void markActiveBreakpoint(const KURL&, int) = 0;
  virtual void unmarkActiveBreakpoint(const KURL&, int) = 0;
  virtual void markDisabledBreakpoint(const KURL&, int) = 0;
  virtual void unmarkDisabledBreakpoint(const KURL&, int) = 0;
  virtual void markExecutionPoint(const KURL&, int) = 0;
  virtual void unmarkExecutionPoint(const KURL&) = 0;
  virtual void markPreExecutionPoint(const KURL&, int) = 0;
  virtual void unmarkPreExecutionPoint(const KURL&) = 0;
  virtual bool hasBreakpointAt(const KURL& , int) = 0;
  virtual void saveExistingFiles() = 0;

  virtual int totalDocuments() = 0;
};

#endif
