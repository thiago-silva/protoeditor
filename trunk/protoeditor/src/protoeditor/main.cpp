/***************************************************************************
 *   Copyright (C) 2004-2006 by Thiago Silva                               *
 *   thiago.silva@kdemail.net                                              *
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

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include <config.h>

#include "mainwindow.h"
#include "protoeditor.h"

static const char description[] =
  I18N_NOOP("A small editor and debugger client");

static const char version[] = VERSION;

static KCmdLineOptions options[] =
  {
    KCmdLineLastOption
  };


int main(int argc, char **argv)
{
  KAboutData about("protoeditor", I18N_NOOP("Protoeditor"), version, description,
                   KAboutData::License_GPL, "(C) 2005-2006 Thiago Silva", 0, "http://protoeditor.sourceforge.net", "thiago.silva@kdemail.com");
  about.addAuthor( "Thiago Silva", 0, "thiago.silva@kdemail.com" );
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions( options );

  KApplication app;

  Protoeditor::self()->init();

  MainWindow *window = Protoeditor::self()->mainWindow();

  if (app.isRestored())
  {
    RESTORE(MainWindow);
  }
  else
  {
    // no session.. just start up normally

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    app.setMainWidget(window);
    window->show();

    args->clear();
  }

  int ret = app.exec();  

  Protoeditor::self()->dispose();
  
  return ret;
}

