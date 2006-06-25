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
#include "dbgsettings.h"
#include "dbgsettingswidget.h"
#include "dbg_defs.h"

#include <qwidget.h>

DBGSettings::DBGSettings(const QString& name)
  : DebuggerSettingsInterface(), m_name(name)
{
  m_widget = 0;
  setCurrentGroup( QString::fromLatin1( m_name ) );

  KConfigSkeleton::ItemBool  *itemBreakOnLoad;
  itemBreakOnLoad = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "BreakOnLoad" ), mBreakOnLoad, true );
  addItem( itemBreakOnLoad, QString::fromLatin1( "BreakOnLoad" ) );
  KConfigSkeleton::ItemBool  *itemSendErrors;
  itemSendErrors = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "SendErrors" ), mSendErrors, true );
  addItem( itemSendErrors, QString::fromLatin1( "SendErrors" ) );
  KConfigSkeleton::ItemBool  *itemSendLogs;
  itemSendLogs = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "SendLogs" ), mSendLogs, true );
  addItem( itemSendLogs, QString::fromLatin1( "SendLogs" ) );
  KConfigSkeleton::ItemBool  *itemSendOutput;
  itemSendOutput = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "SendOutput" ), mSendOutput, true );
  addItem( itemSendOutput, QString::fromLatin1( "SendOutput" ) );
  KConfigSkeleton::ItemBool  *itemSendDetailedOutput;
  itemSendDetailedOutput = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "SendDetailedOutput" ), mSendDetailedOutput, true );
  addItem( itemSendDetailedOutput, QString::fromLatin1( "SendDetailedOutput" ) );
  KConfigSkeleton::ItemInt  *itemListenPort;
  itemListenPort = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "ListenPort" ), mListenPort, DBG_DEFAULT_PORT );
  addItem( itemListenPort, QString::fromLatin1( "ListenPort" ) );

  KConfigSkeleton::ItemBool  *itemEnableJIT;
  itemEnableJIT = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "EnableJIT" ), mEnableJIT, false );
  addItem( itemEnableJIT, QString::fromLatin1( "EnableJIT" ) );

  readConfig();
}

DBGSettings::~DBGSettings()
{
}

void DBGSettings::loadValuesFromWidget()
{
  mBreakOnLoad = m_widget->breakOnLoad();
  mEnableJIT   = m_widget->enableJIT();
  mListenPort  = m_widget->listenPort();
  mSendDetailedOutput = m_widget->sendDetailedOutput();
  mSendErrors = m_widget->sendErrors();
  mSendLogs = m_widget->sendLogs();
  mSendOutput = m_widget->sendOutput();
}

DebuggerTab* DBGSettings::widget()
{
  if(!m_widget) {
    m_widget = new DBGSettingsWidget(this);
  }
  return m_widget;
}
