
#ifndef PLUGIN_KATE_PHP_DEBUGGER_H
#define PLUGIN_KATE_PHP_DEBUGGER_H

#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kate/document.h>
#include <kate/pluginconfiginterface.h>
#include <kate/pluginconfiginterfaceextension.h>
#include <kate/mainwindow.h>
#include <kate/plugin.h>
#include <kate/view.h>
#include <kate/viewmanager.h>

#include <kdialogbase.h>
#include <klibloader.h>
#include <klocale.h>
#include <kurlrequester.h>


namespace Kate {
  class PluginConfigPage;
  class PluginView;
}

class KateUInterface;

class KConfig;
class QStringList;
class LanguageSettings;

class KateSiteSettingsWidget : public Kate::PluginConfigPage
{
  Q_OBJECT
  public:
    KateSiteSettingsWidget(QWidget *parent = 0, const char *name = 0);
    ~KateSiteSettingsWidget();

    void apply();
    void reset ();
    void defaults();
  private:
    class SiteSettingsWidget* m_siteSettingsWidget;
};

class KateExtAppSettingsWidget : public Kate::PluginConfigPage
{
  Q_OBJECT
  public:
    KateExtAppSettingsWidget(QWidget *parent = 0, const char *name = 0);
    ~KateExtAppSettingsWidget();

    void apply();
    void reset ();
    void defaults();
  private:
    class ExtAppSettingsWidget* m_extAppSettingsWidget;
};

class KateLanguageSettingsWidget : public Kate::PluginConfigPage
{
  Q_OBJECT
  public:
    KateLanguageSettingsWidget(LanguageSettings*, QWidget *parent = 0, const char *name = 0);
    ~KateLanguageSettingsWidget();

    void apply();
    void reset ();
    void defaults();
  private:
    class LanguageSettingsWidget* m_langSettingsWidget;
};

class PluginKateDebugger : public Kate::Plugin,
                              Kate::PluginViewInterface,
                              Kate::PluginConfigInterfaceExtension
{
  Q_OBJECT

  public:
    PluginKateDebugger( QObject* parent = 0, const char* name = 0, const QStringList& = QStringList() );
    virtual ~PluginKateDebugger();

    void addView (Kate::MainWindow *win);
    void removeView (Kate::MainWindow *win);

    uint configPages () const;

    Kate::PluginConfigPage *configPage (uint , QWidget *w, const char *name=0);

    QString configPageName(uint) const;

    QString configPageFullName(uint) const;
    QPixmap configPagePixmap (uint /*number = 0*/, int /*size = KIcon::SizeSmall*/) const;

    KConfig *config;

    public slots:
      void slotApplyConfig(class KateTabBarExtensionConfigPage*);
  private:
    KateUInterface *m_ui;   
    int m_views;
    
//     KateSiteSettingsWidget* m_kateSiteSettingsWidget;
//     KateExtAppSettingsWidget* m_kateExtAppSettingsWidget;
// 
//     QValueList<KateLanguageSettingsWidget*> m_kateLangSettingsWidgetList;

};

#endif
