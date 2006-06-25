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
#include "gbsettings.h"
#include "gbsettingswidget.h"
#include "gb_defs.h"

#include <qwidget.h>

GBSettings::GBSettings(const QString& name)
  : DebuggerSettingsInterface(), m_name(name)
{
  m_widget = 0;
  
  setCurrentGroup( QString::fromLatin1( m_name ) );

  KConfigSkeleton::ItemInt  *itemListenPort;
  itemListenPort = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "ListenPort" ), mListenPort, GB_DEFAULT_PORT );
  addItem( itemListenPort, QString::fromLatin1( "ListenPort" ) );

  KConfigSkeleton::ItemBool  *itemEnableJIT;
  itemEnableJIT = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "EnableJIT" ), mEnableJIT, false );
  addItem( itemEnableJIT, QString::fromLatin1( "EnableJIT" ) );

  KConfigSkeleton::ItemString  *itemStartSessionScript;
  itemStartSessionScript = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "StartSessionScript" ), mStartSessionScript, "/Gubed/ServerScripts/StartSession.php");
  addItem( itemStartSessionScript, QString::fromLatin1( "StartSessionScript" ) );

  KConfigSkeleton::ItemBool  *itemBreakOnLoad;
  itemBreakOnLoad = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "BreakOnLoad" ), mBreakOnLoad, true);
  addItem( itemBreakOnLoad, QString::fromLatin1( "BreakOnLoad" ) );

  readConfig();
}

GBSettings::~GBSettings()
{
}

void GBSettings::loadValuesFromWidget()
{
  mEnableJIT   = m_widget->enableJIT();
  mListenPort  = m_widget->listenPort();
  mStartSessionScript = m_widget->startSessionScript();
  mBreakOnLoad        = m_widget->breakOnLoad();
}

DebuggerTab* GBSettings::widget()
{
  if(!m_widget) {
    m_widget = new GBSettingsWidget(this);
  }
  return m_widget;
}

