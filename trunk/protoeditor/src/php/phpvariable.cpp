/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
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

#include "phpvariable.h"


PHPVariable::PHPVariable(Variable* parent)
  : Variable(parent)
{
}
PHPVariable::PHPVariable(QString name)
  : Variable(name)
{
}
PHPVariable::~PHPVariable() {
}

QString PHPVariable::compositeName()
{
  QString cname;
  if(parent()) {
    cname = parent()->compositeName();
    if(parent()->value()->typeName() == "Array") {
      cname += "['" + name() + "']";
    } else {
      cname += "->" + name();
    }
  } else {
    cname = name();
  }

  return cname;
}


//------------------------------------------------------

PHPScalarValue::PHPScalarValue(Variable* owner)
  : VariableScalarValue(owner), m_type(Undefined)
{
}

PHPScalarValue::~PHPScalarValue()
{
}

/*void PHPScalarValue::set(QString value)
{
  m_value = value;
}

QString PHPScalarValue::get()
{
  return m_value;
}
*/

QString PHPScalarValue::typeName()
{
  switch(m_type) {
    case Integer:
      return QString("Integer");
    case Double:
      return QString("Double");
    case Boolean:
      return QString("Boolean");
    case String:
      return QString("String");
    case Resource:
      return QString("Resource");

    /*case Reference:
      return QString("reference");*/
    case Undefined:
    default:
      return VariableValue::UndefinedType;    
  }
}

void PHPScalarValue::setType(int type)
{
  m_type = type;
}

int PHPScalarValue::type()
{
  return m_type;
}

//----------------------------------------------------------------

PHPArrayValue::PHPArrayValue(Variable* owner)
  : VariableListValue(owner)
{
}

PHPArrayValue::~PHPArrayValue()
{
}

QString PHPArrayValue::toString(int indent)
{
  QString ind;
  ind.fill(' ', indent);

  QString s;

  Variable* v;
  for(v =  m_list->first(); v; v = m_list->next()) {
    s += "\n";
    s += ind;

    if(v->isReference()) {
      s += v->name() + " => &" + v->value()->owner()->name();
    } else {
      if(v->value()->isScalar()) {
        s += v->name() + " => " + v->value()->toString();
      } else {
        s += v->name() + " = (" + v->value()->typeName() + ")";
        s += v->value()->toString(indent+3);
      }
    }
  }

  return s;
}

QString PHPArrayValue::typeName()
{
  QString s = "Array[";
  if(m_list) {
    s += QString::number(m_list->count()) + "]";
  } else {
    s += "0]";
  }

  return s;
}

//----------------------------------------------------------------

PHPObjectValue::PHPObjectValue(Variable* owner)
  : VariableListValue(owner)
{
}


PHPObjectValue::~PHPObjectValue()
{
}

QString PHPObjectValue::toString(int indent)
{
  QString ind;
  ind.fill(' ', indent);

  QString s;

  Variable* v;
  for(v =  m_list->first(); v; v = m_list->next()) {
    s += "\n";
    s += ind;

    if(v->isReference()) {
      s += v->name() + " => &" + v->value()->owner()->name();
    } else {
      if(v->value()->isScalar()) {
        s += v->name() + " => " + v->value()->toString();
      } else {
        s += v->name() + " = (" + v->value()->typeName() + ")";
        s += v->value()->toString(indent+3);
      }
    }
  }

  return s;
}

QString PHPObjectValue::typeName()
{
  return m_classType;
}


void PHPObjectValue::setClassType(QString classType)
{
  m_classType = classType;
}
