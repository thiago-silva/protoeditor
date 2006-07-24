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

#ifndef PYTHONVARIABLE_H
#define PYTHONVARIABLE_H

#include "variable.h"


class PythonVariable : public Variable {
public:
  PythonVariable(Variable* parent);
  PythonVariable(QString name);
  virtual ~PythonVariable();

  virtual QString compositeName();  /* ie. house[type], where:
                                         type is this->name()
                                         house is this->parent->name()
                                     */
};

class PythonScalarValue : public VariableScalarValue {
public:
  PythonScalarValue(Variable* owner);
  virtual ~PythonScalarValue();

  void setTypeName(const QString&);

  virtual int type();
  virtual QString typeName();
private:
  QString m_typeName;
};

//---------------------------------------------------------------

class PythonListValue : public VariableListValue {
public:
  PythonListValue(Variable* owner, const QString&);
  virtual ~PythonListValue();

  enum { List, Tuple, Dictionary };

  virtual int     type();
  virtual QString typeName();
  virtual QString toString(int indent = 0);
private:  
  QString m_typeName;
  int m_type;
};


#endif
