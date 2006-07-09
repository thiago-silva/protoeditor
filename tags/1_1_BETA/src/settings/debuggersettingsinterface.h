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

class LanguageSettings;

//------------------------------------------------------------------

class DebuggerTab : public QWidget {
public:
  DebuggerTab(QWidget* parent = 0, const char* name = 0) : QWidget(parent, name) {}

  virtual ~DebuggerTab() {}

  virtual void setLangEnabled(bool) = 0;
  virtual void populate()           = 0;
};

//------------------------------------------------------------------

class DebuggerSettingsInterface : public KConfigSkeleton {
public:

  DebuggerSettingsInterface(const QString& name, const QString& label, LanguageSettings* langs)
      :  KConfigSkeleton(QString::fromLatin1("protoeditorrc")),
         m_name(name), m_label(label), m_langsettings(langs) {}
  
  virtual ~DebuggerSettingsInterface() {}

  LanguageSettings*    languageSettings()
  {
    return m_langsettings;
  }

  QString debuggerName() const 
  {
    return m_name;
  }

  QString debuggerLabel() const
  {
    return m_label;
  }

  virtual void         loadValuesFromWidget() = 0;
  virtual DebuggerTab* widget()               = 0;

private:
  QString m_name;
  QString m_label;
  LanguageSettings* m_langsettings;
};

#endif
