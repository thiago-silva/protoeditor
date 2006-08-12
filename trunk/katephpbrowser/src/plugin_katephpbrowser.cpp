/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "plugin_katephpbrowser.h"

#include <klocale.h>
#include <kstandarddirs.h>
#include <kate/toolviewmanager.h>
#include <kiconloader.h>
#include <kgenericfactory.h>

#include "browsertab.h"

K_EXPORT_COMPONENT_FACTORY( katephpbrowserplugin, KGenericFactory<PluginKatePHPBrowser>( "katephpbrowser" ) )

class PluginView : public KXMLGUIClient
{
  friend class PluginKatePHPBrowser;

  public:
    Kate::MainWindow *win;
    QWidget* dock;
};

PluginKatePHPBrowser::PluginKatePHPBrowser( QObject* parent, const char* name, const QStringList&)
    : Kate::Plugin ( (Kate::Application*)parent, name )
{
}

PluginKatePHPBrowser::~PluginKatePHPBrowser()
{
}

void PluginKatePHPBrowser::addView(Kate::MainWindow *win)
{
  /// @todo doesn't this have to be deleted?
  PluginView *view = new PluginView ();

  Kate::ToolViewManager *tool_view_manager = win->toolViewManager();

  QWidget* dock = tool_view_manager->createToolView("kate_plugin_phpbrowser",
      Kate::ToolViewManager::Left, SmallIcon("gear"), i18n("PHP Browser"));

  (void)new BrowserTab(application(), dock);

   view->win = win ;
   view->dock = dock;
   m_views.append (view);
}
void PluginKatePHPBrowser::removeView(Kate::MainWindow *win)
{
  for (uint z=0; z < m_views.count(); z++)
    if (m_views.at(z)->win == win)
    {
      PluginView *view = m_views.at(z);
      m_views.remove (view);
      delete view->dock;
      delete view;
    }
}

#include "plugin_katephpbrowser.moc"
