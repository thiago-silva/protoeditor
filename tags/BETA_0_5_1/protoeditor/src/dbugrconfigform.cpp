/***************************************************************************
 *   Copyright (C) 2004 by Thiago Silva                                    *
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



#include "dbugrconfigform.h"
#include "debuggerconfigurations.h"
#include "dbgconfiguration.h"
#include <qmessagebox.h>
#include <qbuttongroup.h>
#include <qlineedit.h>

DebuggerConfigForm::DebuggerConfigForm(QWidget* parent, const char* name, bool modal, WFlags fl)
    : DebuggerConfigFormBase(parent,name, modal,fl), m_options_changed(false)
{}

DebuggerConfigForm::~DebuggerConfigForm()
{}

void DebuggerConfigForm::confirmed()
{
  m_options_changed = true;
  close();
}

void DebuggerConfigForm::canceled()
{
  m_options_changed = false;
  close();
}

bool DebuggerConfigForm::optionsChanged()
{
  return m_options_changed;
}

DebuggerConfigurations* DebuggerConfigForm::configurations()
{
  DebuggerConfigurations* conf = new DebuggerConfigurations();

  switch(bt_group->id(bt_group->selected()))
  {
    case GUBED:
    break;
    case DBG:
    conf->setDebuggerId(DBG);
    conf->dbgConfiguration()->setLocalBaseDir(ed_dbg_localBasedir->text());
    conf->dbgConfiguration()->setServerBaseDir(ed_dbg_serverBaseDir->text());
    conf->dbgConfiguration()->setListenPort(ed_dbg_listenPort->text().toInt());
    conf->dbgConfiguration()->setHost(ed_dbg_host->text());
    break;
    case XDEBUG:
    break;
  }

  return conf;
}

void DebuggerConfigForm::setConfigurations(DebuggerConfigurations* conf)
{
  if(conf->debuggerId() == -1) return;

  bt_group->setButton(conf->debuggerId());

  switch(conf->debuggerId())
  {
    case GUBED:
    break;
    case DBG:
    ed_dbg_localBasedir->setText(conf->dbgConfiguration()->localBaseDir());
    ed_dbg_serverBaseDir->setText(conf->dbgConfiguration()->serverBaseDir());
    ed_dbg_listenPort->setText(QString::number(conf->dbgConfiguration()->listenPort()));
    ed_dbg_host->setText(conf->dbgConfiguration()->host());
    break;
    case XDEBUG:
    break;
  }
}

#include "dbugrconfigform.moc"
