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
#ifndef DEBUGGERSETTINGSINTERFACE_H
#define DEBUGGERSETTINGSINTERFACE_H

#include <qwidget.h>

#include <qstring.h>
#include <kconfigskeleton.h>


//------------------------------------------------------------------

class DebuggerTab : public QWidget {
public:
  DebuggerTab(QWidget* parent = 0, const char* name = 0) : QWidget(parent, name) {}

  virtual ~DebuggerTab() {}

  virtual void populate() = 0;
};

//------------------------------------------------------------------

class DebuggerSettingsInterface : public KConfigSkeleton {
public:

  DebuggerSettingsInterface()
      :  KConfigSkeleton(QString::fromLatin1("protoeditorrc")) {}

  virtual ~DebuggerSettingsInterface() {}

  virtual QString      name()                 = 0;
  virtual void         loadValuesFromWidget() = 0;
  virtual DebuggerTab* widget()               = 0;
};

#endif
