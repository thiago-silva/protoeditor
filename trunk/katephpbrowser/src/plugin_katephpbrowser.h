/***************************************************************************
 *   Copyright (C) 2006 by Thiago Silva   *
 *   tbls@brturbo.com.br   *
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


#ifndef _PLUGIN_KATEPHPBROWSER_H_
#define _PLUGIN_KATEPHPBROWSER_H_

#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kate/document.h>
#include <kate/mainwindow.h>
#include <kate/plugin.h>
#include <kate/view.h>
#include <kate/viewmanager.h>

#include <klibloader.h>
#include <klocale.h>

class PluginKatePHPBrowser : public Kate::Plugin, Kate::PluginViewInterface
{
  Q_OBJECT

  public:
    PluginKatePHPBrowser(QObject* parent = 0, const char* name = 0, const QStringList& = QStringList());
    virtual ~PluginKatePHPBrowser();

    void addView (Kate::MainWindow *win);
    void removeView (Kate::MainWindow *win);

  private:
    QPtrList<class PluginView> m_views;
};

#endif // _PLUGIN_KATEPHPBROWSER_H_
