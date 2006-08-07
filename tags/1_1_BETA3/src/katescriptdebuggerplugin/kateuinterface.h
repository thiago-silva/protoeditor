#ifndef KATEUINTERFACE_H
#define KATEUINTERFACE_H

#include <qobject.h>

#include "uinterface.h"
#include <kxmlguiclient.h>

class KateEditorUI;
class KateDebuggerUI;

class KHistoryCombo;

class KSelectAction;
class KToggleAction;
class KAction;
class KPopupMenu;
class PluginKateDebugger;

class KateUInterface;

namespace Kate {
  class MainWindow;
}

class KateUInterface : public QObject,
                       public KXMLGUIClient,
                       public UInterface
                       
{
  Q_OBJECT
public:
  KateUInterface(PluginKateDebugger*, Kate::MainWindow *w);
  ~KateUInterface();

  void setup();

  EditorInterface*    editorUI();
  DebuggerInterface*  debuggerUI();

  void setDebugMsg(const QString&);
  void setDebuggerName(const QString&);
  void setLedState(int);


  KHistoryCombo*      argumentCombo();

  void actionStateChanged(const QString&);

  void changeCaption(const QString&);
  void saveArgumentList();  

  QString currentSiteName();
  void setSiteNames(const QStringList&);
  void setCurrentSite(int);

  void setCurrentLanguage(const QString&);

  bool isCurrentScriptActionChecked();

  void saveRecentEntries();

  void clearLanguages();
  void addLanguage(const QString&);

  QPopupMenu* editorPopupMenu();

  KAction* editorAddWatchAction();  

  void openFile();
  void openFile(const KURL& url);

  bool saveCurrentFile();
  bool saveCurrentFileAs();

  PluginKateDebugger* plugin();
  Kate::MainWindow* window();
signals:
  void sigExecuteScript(const QString&);

  void sigDebugScript(const QString&);
  void sigDebugScript();

  void sigProfileScript(const QString&);
  void sigProfileScript();


public slots:
  void slotAcFocusArgumentBar();

  void slotAcExecuteScript();
  void slotAcDebugStart();
  void slotAcProfileScript();

  void slotAcCurrentSiteChanged();

  void slotAcChangeCurrentLanguage(int);

  void slotAcUseCurrentScript(bool);

private:
  void createWidgets();
  void setupActions();

  PluginKateDebugger* m_plugin;
  Kate::MainWindow *m_win;
  QWidget *m_dock;

  KateEditorUI* m_editorUI;
  KateDebuggerUI* m_debuggerUI;

  //toolbar
  KHistoryCombo       *m_cbArguments;

  KSelectAction       *m_siteAction;
  KToggleAction       *m_activeScriptAction;
  KAction             *m_debugAction;

  KPopupMenu          *m_langPopup;
  QString             m_lastLang;
  QMap<int, QString>  m_langMap;                
};

#endif
