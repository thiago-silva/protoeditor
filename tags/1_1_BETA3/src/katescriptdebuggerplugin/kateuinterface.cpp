#include "kateuinterface.h"

#include "kateeditorui.h"
#include "katedebuggerui.h"
#include <kate/mainwindow.h>
#include <klocale.h>

#include <kaction.h>
#include <ktoolbarbutton.h>
#include <kcombobox.h>
#include <qlabel.h>

#include "sitesettings.h"
#include "protoeditor.h"
#include "protoeditorsettings.h"

#include "plugin_katescriptdebugger.h"

#include <kpopupmenu.h>
#include <ktoolbar.h>

KateUInterface::KateUInterface(PluginKateDebugger* p, Kate::MainWindow *w)
  : m_plugin(p), m_win(w)
{

}

KateUInterface::~KateUInterface()
{
  m_editorUI->setTerminating();
  delete m_editorUI;
  delete m_debuggerUI;
}

void KateUInterface::setup()
{
  createWidgets();
  setupActions();

  setInstance(new KInstance("kate"));
  setXMLFile( "plugins/katescriptdebugger/ui.rc" );
  m_win->guiFactory()->addClient(this);

  stateChanged("init");

  m_langPopup = new KPopupMenu();  
  connect(m_langPopup, SIGNAL(activated(int)), this, SLOT(slotAcChangeCurrentLanguage(int)));


  KToolBar* bar = dynamic_cast<KToolBar*>
        (m_win->guiFactory()->container("debug_toolbar", this));

  if (bar) 
  {
    KToolBarButton* button;
    for(int i = 0; i < bar->count(); i++) {
      button = bar->getButton(bar->idAt(i));
      if(button && QString(button->name()).contains("script_execute")) {
        button->setDelayedPopup(m_langPopup);
      }
    }
  }
}

void KateUInterface::createWidgets()
{
  m_debuggerUI = new KateDebuggerUI(this);

  m_editorUI = new KateEditorUI(this);
  m_editorUI->setup();

  m_cbArguments = new KHistoryCombo(true, m_win->centralWidget());

  //connects the EditorUI to DebuggerUI
  connect(m_editorUI, SIGNAL(sigBreakpointMarked(const KURL&, int, bool)),
    m_debuggerUI, SLOT(slotBreakpointMarked(const KURL&, int, bool)));

  connect(m_editorUI, SIGNAL(sigBreakpointUnmarked(const KURL&, int)),
    m_debuggerUI, SLOT(slotBreakpointUnmarked(const KURL&, int)));
}

void KateUInterface::setupActions()
{

//   //menu "script"
  m_siteAction  = new KSelectAction(i18n("Site"), 0, actionCollection(), "site_selection");
  m_siteAction->setComboWidth(150);

  connect(m_siteAction, SIGNAL(activated()),
      this, SLOT(slotAcCurrentSiteChanged()));

  m_activeScriptAction = new KToggleAction(i18n("Use Current Script"), "attach", 0, actionCollection(), "use_current_script");

  connect(m_activeScriptAction, SIGNAL(toggled (bool)),
      this, SLOT(slotAcUseCurrentScript(bool)));

  (void) new KAction(i18n("Execute in Console"), "gear", "Shift+F9", 
                      this, SLOT(slotAcExecuteScript()), actionCollection(), "script_execute");

  m_debugAction = new KAction(i18n("Start Debug"), "dbgstart", "F9", this,
                    SLOT(slotAcDebugStart()), actionCollection(), "debug_start");


  (void)new KAction(i18n("Stop Debug"), "stop", "Escape", Protoeditor::self(),
                    SLOT(slotAcDebugStop()), actionCollection(), "debug_stop");

  (void)new KAction(i18n("Run to Cursor"), "dbgrunto", 0, Protoeditor::self(),
                    SLOT(slotAcDebugRunToCursor()), actionCollection(), "debug_run_to_cursor");


  (void)new KAction(i18n("Step Over"), "dbgnext", "F10", Protoeditor::self(),
                    SLOT(slotAcDebugStepOver()), actionCollection(), "debug_step_over");

  (void)new KAction(i18n("Step Into"), "dbgstep", "F11", Protoeditor::self(),
                    SLOT(slotAcDebugStepInto()), actionCollection(), "debug_step_into");


  (void)new KAction(i18n("Step Out"), "dbgstepout", "F12", Protoeditor::self(),
                    SLOT(slotAcDebugStepOut()), actionCollection(), "debug_step_out");

 (void)new KAction(i18n("Profile (PHP+DBG)"), "math_sum", "Alt+P", 
      this, SLOT(slotAcProfileScript()), actionCollection(), "script_profile");

  (void)new KAction(i18n("Toggle Breakpoint"), "activebreakpoint", "Alt+B", Protoeditor::self(),
                    SLOT(slotAcDebugToggleBp()), actionCollection(), "debug_toggle_bp");


  KWidgetAction* comboAction = new KWidgetAction(argumentCombo(), i18n("Argument Bar "), 0,
                  0, 0, actionCollection(), "argument_combobox" );

  comboAction->setShortcutConfigurable(false);
  comboAction->setAutoSized(true);

  (void)new KAction(i18n("Clear Arguments"), "clear_left", 0, argumentCombo(),
                    SLOT(clearEdit()), actionCollection(), "argument_clear");

  (void)new KWidgetAction(new QLabel(i18n("Arguments: "), m_win->centralWidget()), i18n("Arguments "), "Alt+a", 
      this, SLOT(slotAcFocusArgumentBar()), actionCollection(), "argument_label" );
  

  connect(this, SIGNAL(sigExecuteScript(const QString&)), 
      Protoeditor::self(), SLOT(slotAcExecuteScript(const QString&)));

  connect(this, SIGNAL(sigDebugScript(const QString&)), 
      Protoeditor::self(), SLOT(slotAcDebugStart(const QString&)));

  connect(this, SIGNAL(sigDebugScript()), 
      Protoeditor::self(), SLOT(slotAcDebugStart()));

  connect(this, SIGNAL(sigProfileScript(const QString&)), 
      Protoeditor::self(), SLOT(slotAcProfileScript(const QString&)));
}



EditorInterface*    KateUInterface::editorUI()
{
  return m_editorUI; 
}

DebuggerInterface* KateUInterface::debuggerUI()
{
  return m_debuggerUI;
}

void KateUInterface::setDebugMsg(const QString&)
{
  //TODO
  //statusBar()->setDebugMsg(msg);
}

void KateUInterface::setDebuggerName(const QString&)
{
  //TODO
  //statusBar()->setDebuggerName(name);
}

void KateUInterface::setLedState(int)
{
  //TODO
  //statusBar()->setLedState(state);
}

KHistoryCombo*  KateUInterface::argumentCombo()
{
  return m_cbArguments;
}

void KateUInterface::actionStateChanged(const QString& state)
{
  if(state == "debug_started")
  {
    actionCollection()->action("debug_start")->setText(i18n("Continue"));
    actionCollection()->action("site_selection")->setEnabled(false);
  } 
  else if(state == "debug_stopped")
  {
    actionCollection()->action("site_selection")->setEnabled(true);
    actionCollection()->action("debug_start")->setText(i18n("Start Debug"));
  }
  else if((state == "has_nofileopened") || (state == "has_fileopened"))
  {
    return;
  }
    
  stateChanged(state);
}

void KateUInterface::changeCaption(const QString&)
{
  //TODO
}

void KateUInterface::saveArgumentList()
{
  QString arg = argumentCombo()->currentText();
  if(!arg.isEmpty()) 
  {
    argumentCombo()->addToHistory(arg);
  }
}

QString KateUInterface::currentSiteName()
{
  return m_siteAction->currentText();
}

void KateUInterface::setSiteNames(const QStringList& sites)
{
  m_siteAction->setItems(sites);
}

void KateUInterface::setCurrentSite(int idx) 
{
  m_siteAction->setCurrentItem(idx);  
  slotAcCurrentSiteChanged();
}

void KateUInterface::setCurrentLanguage(const QString& lang)
{
  QMap<int, QString>::iterator it;
  for(it = m_langMap.begin(); it != m_langMap.end(); ++it) 
  {
    if(it.data() == lang)
    {
      slotAcChangeCurrentLanguage(it.key());
    }
  }
}

bool KateUInterface::isCurrentScriptActionChecked()
{
  return m_activeScriptAction->isChecked();
}

void KateUInterface::saveRecentEntries()
{
//   m_actionRecent->saveEntries(m_plugin->config);
}

void KateUInterface::clearLanguages()
{
  m_langPopup->clear();
  m_lastLang = QString::null;
}

void KateUInterface::addLanguage(const QString& langName)
{
  int id = m_langPopup->count() + 1;
  m_langPopup->insertItem(langName, id);

  m_langMap[id] = langName;
}

void KateUInterface::openFile()
{
  //TODO
  //I think this will never be called (called only when no document is opened)
}

void KateUInterface::openFile(const KURL& url)
{
  m_editorUI->openDocument(url);  
}


bool KateUInterface::saveCurrentFile()
{
  return m_editorUI->saveCurrentFile();
}

bool KateUInterface::saveCurrentFileAs()
{
  return m_editorUI->saveCurrentFile();
}

void KateUInterface::slotAcFocusArgumentBar()
{
  m_cbArguments->setFocus();
  m_cbArguments->lineEdit()->selectAll();
}

void KateUInterface::slotAcExecuteScript()
{
  emit sigExecuteScript(m_lastLang);
}

void KateUInterface::slotAcDebugStart()
{
  emit sigDebugScript(m_lastLang);
}
void KateUInterface::slotAcProfileScript()
{
  emit sigProfileScript(m_lastLang);
}

void KateUInterface::slotAcCurrentSiteChanged()
{
  Protoeditor::self()->settings()->setCurrentSiteName(currentSiteName());

  if(isCurrentScriptActionChecked()) return;

  KToolBar* bar = dynamic_cast<KToolBar*>
        (m_win->guiFactory()->container("debug_toolbar", this));

  KToolBarButton* button;  

  if((currentSiteName() == ProtoeditorSettings::LocalSiteName)) 
  {
    for(int i = 0; i < bar->count(); i++) {
      button = bar->getButton(bar->idAt(i));
      if(button && QString(button->name()).contains("debug_start")) {
        button->setDelayedPopup(m_langPopup);
      }
    }
  }
  else
  {
    for(int i = 0; i < bar->count(); i++) {
      button = bar->getButton(bar->idAt(i));
      if(button && QString(button->name()).contains("debug_start")) {
        button->setDelayedPopup(0);
      }
    }
  }
}

void KateUInterface::slotAcChangeCurrentLanguage(int id)
{
  m_lastLang = m_langMap[id];

  Protoeditor::self()->settings()->setCurrentLanguage(m_lastLang);

  for(unsigned int i = 0; i < m_langPopup->count(); i++)
  {
    m_langPopup->setItemChecked(m_langPopup->idAt(i), false);
  }

  m_langPopup->setItemChecked(id, !m_langPopup->isItemChecked(id));

  //TODO
//   statusBar()->setDebuggerName(m_lastLang);
}

void KateUInterface::slotAcUseCurrentScript(bool)
{
  KToolBar* bar = dynamic_cast<KToolBar*>
        (m_win->guiFactory()->container("debug_toolbar", this));

  KToolBarButton* button;

  if(!isCurrentScriptActionChecked() && (currentSiteName() != ProtoeditorSettings::LocalSiteName)) 
  {
    for(int i = 0; i < bar->count(); i++) {
      button = bar->getButton(bar->idAt(i));
      if(button && QString(button->name()).contains("debug_start")) {
        button->setDelayedPopup(0);
      }
    }
  }
  else
  {
    for(int i = 0; i < bar->count(); i++) {
      button = bar->getButton(bar->idAt(i));
      if(button && QString(button->name()).contains("debug_start")) {
        button->setDelayedPopup(m_langPopup);
      }
    }
  }
}

PluginKateDebugger* KateUInterface::plugin()
{
  return m_plugin;
}

Kate::MainWindow* KateUInterface::window()
{
  return m_win;
}

#include "kateuinterface.moc"
