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

#include "statusbarwidget.h"

#include <qlabel.h>
#include <kiconloader.h>

StatusBarWidget::StatusBarWidget(QWidget* parent, const char *name)
  : KStatusBar(parent, name)
{
  //first item: debug led
  m_lbLed = new QLabel(this);
  m_lbLed->setPixmap(QPixmap(UserIcon("indicator_off")));
  m_lbLed->setAlignment( Qt::AlignCenter );
  m_lbLed->setSizePolicy(QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ));
  this->addWidget(m_lbLed);

  //second item: debugger name
  m_lbDebugName = new QLabel(this);
  m_lbDebugName->setMinimumSize( 40, 0 );
  //m_lbDebugMsg->setSizePolicy(QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ));
  this->addWidget(m_lbDebugName, 0 /*stretch*/, false );
  
  //third item: debug msgs
  m_lbDebugMsg = new QLabel(this);
  m_lbDebugMsg->setMinimumSize( 0, 0 );
  m_lbDebugMsg->setSizePolicy(QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ));
  this->addWidget(m_lbDebugMsg, 1 /*stretch*/, false );

  //fourth item: editor msgs
  m_lbEditorMsg = new QLabel(this);
  m_lbEditorMsg->setMinimumSize( 230, 0 );
  m_lbEditorMsg->setSizePolicy(QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ));
  m_lbEditorMsg->setAlignment( Qt::AlignCenter );
  this->addWidget(m_lbEditorMsg, 1 /*stretch*/, false );
}


StatusBarWidget::~StatusBarWidget()
{
}

void StatusBarWidget::setEditorStatus(const QString& msg)
{
  m_lbEditorMsg->setText(msg);
}

void StatusBarWidget::setDebugMsg(const QString& msg)
{
  m_lbDebugMsg->setText(msg);
}

void StatusBarWidget::setDebuggerName(const QString& name)
{
  m_lbDebugName->setText(name);
}

void StatusBarWidget::setLedState(int state)
{
  switch(state)
  {
    case LedOn:
      m_lbLed->setPixmap(QPixmap(UserIcon("indicator_on")));
      break;
/*    case LedWait:
      m_lbLed->setPixmap(QPixmap(UserIcon("indicator_wait")));
      break;*/
    case LedOff:
    default:
      m_lbLed->setPixmap(QPixmap(UserIcon("indicator_off")));
  }
}

#include "statusbarwidget.moc"
