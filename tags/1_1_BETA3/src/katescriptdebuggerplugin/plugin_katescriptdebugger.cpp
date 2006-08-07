
#include "plugin_katescriptdebugger.h"
#include "plugin_katescriptdebugger.moc"

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qstringlist.h>
#include <qwhatsthis.h>
#include <qwidget.h>

#include <kaction.h>
#include <kanimwidget.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kgenericfactory.h>
#include <kapplication.h>
#include <qvbox.h>

#include "protoeditor.h"
#include "kateuinterface.h"

#include "protoeditorsettings.h"
#include "languagesettings.h"
#include "sitesettingswidget.h"
#include "extappsettingswidget.h"

K_EXPORT_COMPONENT_FACTORY( katescriptdebuggerplugin, KGenericFactory<PluginKateDebugger>( "katescriptdebugger" ) )

//BEGIN PluginKateDebugger
PluginKateDebugger::PluginKateDebugger( QObject* parent, const char* name, const QStringList& )
    : Kate::Plugin ( (Kate::Application *)parent, name ), m_ui(0), m_views(0)
//      m m_kateSiteSettingsWidget(0), m_kateExtAppSettingsWidget(0)
{  

}

PluginKateDebugger::~PluginKateDebugger()
{
//   Protoeditor::dispose();
}

uint PluginKateDebugger::configPages () const
{
  //extapp + sites + languages
  return 2 + Protoeditor::self()->settings()->languageSettingsList().count();
}

void PluginKateDebugger::addView(Kate::MainWindow *w)
{
  if(m_views == 0) 
  {
    m_ui = new KateUInterface(this, w);
    Protoeditor::self()->init(m_ui, "katescriptdebuggerrc");
  }

  m_views++;
}

void PluginKateDebugger::removeView(Kate::MainWindow *win)
{
  if(m_ui->window() == win)
  {
    Protoeditor::dispose();
    win->guiFactory()->removeClient(m_ui);
    delete m_ui;
  }
}

/* Informs the tab <ID> */
Kate::PluginConfigPage* PluginKateDebugger::configPage (uint id,
                                  QWidget *w, const char */*name*/)
{
  Kate::PluginConfigPage* page;

  QValueList<LanguageSettings*> llist;
  QValueList<LanguageSettings*>::iterator it;

  switch(id) {
    case 0:
        page = new KateSiteSettingsWidget(w);
        break;
    case 1:
        page = new KateExtAppSettingsWidget(w);
        break;
    default:
        llist = Protoeditor::self()->settings()->languageSettingsList();
        page = new KateLanguageSettingsWidget(llist[id-2], w);
  }
  return page;
}

QString PluginKateDebugger::configPageName(uint id) const 
{ 
  switch(id) 
  {
    case 0:
      return i18n("Debugger Sites"); 
    case 1:      
      return i18n("Debugger External Applications");
  }

  //language names
  QValueList<LanguageSettings*> llist = Protoeditor::self()->settings()->languageSettingsList();
  QValueList<LanguageSettings*>::iterator it;
  it = llist.at(id-2);
  return i18n("Debugger ") + (*it)->languageName();
}

QString PluginKateDebugger::configPageFullName(uint id) const 
{
  switch(id) 
  {
    case 0:
      return i18n("Sites");
    case 1:
      return i18n("External Applications");
  }

  //language names
  QValueList<LanguageSettings*> llist = Protoeditor::self()->settings()->languageSettingsList();
  QValueList<LanguageSettings*>::iterator it;
  it = llist.at(id-2);
  return (*it)->languageName();
}

QPixmap PluginKateDebugger::configPagePixmap (uint id, int size) const
{
  switch(id) 
  {
    case 0:
      return BarIcon("package_network", size);
    case 1:
      return BarIcon("gear", size);
  }

  //language icons
  QValueList<LanguageSettings*> llist = Protoeditor::self()->settings()->languageSettingsList();
  QValueList<LanguageSettings*>::iterator it;
  it = llist.at(id-2);
  return BarIcon((*it)->iconName(), size);
}

void PluginKateDebugger::slotApplyConfig(KateTabBarExtensionConfigPage*)
{
  Protoeditor::self()->settings()->writeConfig();
}

//END plugin


//BEGIN Settings widgets

KateSiteSettingsWidget::KateSiteSettingsWidget(QWidget *parent, const char */*name*/)
  : Kate::PluginConfigPage( parent )
{
  m_siteSettingsWidget = new SiteSettingsWidget(this);

  QVBoxLayout* lo = new QVBoxLayout( this );
  lo->addWidget(m_siteSettingsWidget);

  m_siteSettingsWidget->populate();
}

KateSiteSettingsWidget::~KateSiteSettingsWidget() 
{
}

void KateSiteSettingsWidget::apply()
{
  m_siteSettingsWidget->updateSettings();
  Protoeditor::self()->settings()->writeConfig();
}

void KateSiteSettingsWidget::reset()
{
}

void KateSiteSettingsWidget::defaults()
{
}

KateExtAppSettingsWidget::KateExtAppSettingsWidget(QWidget *parent, const char */*name*/)
  : Kate::PluginConfigPage( parent )
{
  m_extAppSettingsWidget = new ExtAppSettingsWidget(this);

  QVBoxLayout* lo = new QVBoxLayout( this );
  lo->addWidget(m_extAppSettingsWidget);

  m_extAppSettingsWidget->populate();
}

KateExtAppSettingsWidget::~KateExtAppSettingsWidget()
{
}

void KateExtAppSettingsWidget::apply()
{
  m_extAppSettingsWidget->updateSettings();
  Protoeditor::self()->settings()->writeConfig();
}

void KateExtAppSettingsWidget::reset ()
{
}

void KateExtAppSettingsWidget::defaults()
{
}

KateLanguageSettingsWidget::KateLanguageSettingsWidget(LanguageSettings* ls, QWidget *parent, const char */*name*/)
  : Kate::PluginConfigPage( parent )
{
  QVBoxLayout* lo = new QVBoxLayout( this );
  QVBox* box = new QVBox(this);
  lo->addWidget(box);
  m_langSettingsWidget = ls->createSettingsWidget(box);
  m_langSettingsWidget->populate();
}

KateLanguageSettingsWidget::~KateLanguageSettingsWidget()
{
}

void KateLanguageSettingsWidget::apply()
{
  m_langSettingsWidget->updateSettings();
  Protoeditor::self()->settings()->writeConfig();
}

void KateLanguageSettingsWidget::reset ()
{
}

void KateLanguageSettingsWidget::defaults()
{
}

//END Settings widgets
