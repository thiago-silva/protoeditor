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


#ifndef DEBUGGERCONFIGFORM_H
#define DEBUGGERCONFIGFORM_H

#include "dbugrconfigformbase.h"

class DebuggerConfigurations;

class DebuggerConfigForm : public DebuggerConfigFormBase
{
  Q_OBJECT

public:
  DebuggerConfigForm(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DebuggerConfigForm();

  bool optionsChanged();

  DebuggerConfigurations* configurations();
  void setConfigurations(DebuggerConfigurations*);

public slots:
  virtual void confirmed();
  virtual void canceled();

private:
  void updateConfigurations();

  bool m_options_changed;
};

#endif

