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

#ifndef PHPVARIABLE_H
#define PHPVARIABLE_H

#include "variable.h"


class PHPVariable : public Variable {
public:
  PHPVariable(Variable* parent);
  PHPVariable(QString name);
  virtual ~PHPVariable();

  virtual QString compositeName();  /* ie. house[type], where:
                                         type is this->name()
                                         house is this->parent->name()
                                     */
};

class PHPScalarValue : public VariableScalarValue {
public:
  enum { Integer, Double, Boolean, String, Resource, /*Reference, Object, */ Undefined };

  PHPScalarValue(Variable* owner);
  virtual ~PHPScalarValue();

  virtual void setType(int);
  virtual int type();

  virtual QString typeName();
private:
  int m_type;
};

//---------------------------------------------------------------

class PHPArrayValue : public VariableListValue {
public:
  PHPArrayValue(Variable* owner);
  virtual ~PHPArrayValue();

  virtual QString toString(int indent = 0);
  virtual QString typeName();
};

class PHPObjectValue : public VariableListValue {
public:
  PHPObjectValue(Variable* owner);
  virtual ~PHPObjectValue();

  virtual QString toString(int indent = 0);
  virtual QString typeName();
  virtual void setClassType(QString);

private:
  QString m_classType;
};

#endif
