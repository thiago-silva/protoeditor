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

#include "xdsettings.h"
#include "xdsettingswidget.h"

#include <qwidget.h>

XDSettings::XDSettings(const QString& name)
  : DebuggerSettingsInterface(), m_name(name)
{
  m_widget = 0;
  
  setCurrentGroup( QString::fromLatin1( m_name ) );

  KConfigSkeleton::ItemInt  *itemListenPort;
  itemListenPort = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "ListenPort" ), mListenPort, 9000 );
  addItem( itemListenPort, QString::fromLatin1( "ListenPort" ) );

  KConfigSkeleton::ItemBool  *itemEnableJIT;
  itemEnableJIT = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "EnableJIT" ), mEnableJIT, true );
  addItem( itemEnableJIT, QString::fromLatin1( "EnableJIT" ) );

  
  KConfigSkeleton::ItemBool  *itemSendSuperGlobals;
  itemSendSuperGlobals = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "SendSuperGlobals" ), mSendSuperGlobals, true);
  addItem( itemSendSuperGlobals, QString::fromLatin1( "SendSuperGlobals" ) );
  
  KConfigSkeleton::ItemBool  *itemBreakOnLoad;
  itemBreakOnLoad = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "BreakOnLoad" ), mBreakOnLoad, true);
  addItem( itemBreakOnLoad, QString::fromLatin1( "BreakOnLoad" ) );

  readConfig();
}

XDSettings::~XDSettings()
{
}

void XDSettings::loadValuesFromWidget()
{
  mEnableJIT        = m_widget->enableJIT();
  mListenPort       = m_widget->listenPort();
  mSendSuperGlobals = m_widget->sendSuperGlobals();
  mBreakOnLoad      = m_widget->breakOnLoad();
}

DebuggerTab* XDSettings::widget()
{
  if(!m_widget) {
    m_widget = new XDSettingsWidget(this);
  }
  return m_widget;
}
