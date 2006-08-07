#ifndef UINTERFACE_H
#define UINTERFACE_H

class QString;
class QStringList;
class KURL;

class EditorInterface;
class DebuggerInterface;
class StatusBarWidget;
class KHistoryCombo;

class QPopupMenu;
class KAction;
class KXMLGUIClient;

class UInterface 
{

public:
  virtual ~UInterface();

  virtual void setup() = 0;

  virtual EditorInterface*    editorUI() = 0;
  virtual DebuggerInterface*  debuggerUI() = 0;

  //status methods
  virtual void setDebugMsg(const QString&) = 0;
  virtual void setDebuggerName(const QString&) = 0;
  virtual void setLedState(int) = 0;

  virtual KHistoryCombo*      argumentCombo() = 0;

  virtual void actionStateChanged(const QString&) = 0;

  virtual void changeCaption(const QString&) = 0;
  virtual void saveArgumentList() = 0;  

  virtual QString currentSiteName() = 0;
  virtual void setSiteNames(const QStringList&) = 0;
  virtual void setCurrentSite(int) = 0;

  virtual void setCurrentLanguage(const QString&) = 0;

  virtual bool isCurrentScriptActionChecked() = 0;

  virtual void saveRecentEntries() = 0;

  virtual void clearLanguages() = 0;
  virtual void addLanguage(const QString&) = 0;

  virtual void openFile() = 0;
  virtual void openFile(const KURL& url) = 0;

  virtual bool saveCurrentFile() = 0;
  virtual bool saveCurrentFileAs() = 0;
};


#endif
