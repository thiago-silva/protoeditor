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

#include "pythonvariable.h"
#include <klocale.h>

PythonVariable::PythonVariable(Variable* parent)
  : Variable(parent)
{
}
PythonVariable::PythonVariable(QString name)
  : Variable(name)
{
}
PythonVariable::~PythonVariable() {
}

QString PythonVariable::compositeName()
{
  return perlCompositeName(false);
}

QString PythonVariable::perlCompositeName(bool eval)
{
  if(!parent()) 
  {
    return perlName(eval);
  }
  else
  {
    QString cname = dynamic_cast<PythonVariable*>(parent())->perlCompositeName(true);
    return cname + name();
  }
}
  

QString PythonVariable::perlName(bool eval)
{
  if(!eval)
  {
    return name();
  }
  else
  {
    switch(value()->type())
    {
      case PythonListValue::Array:
        return name().replace('@', '$');
      case PythonListValue::Hash:
        return name().replace('%', '$');
      case PythonListValue::Object:
        return name();
    }
  }
  return QString::null;
}

//------------------------------------------------------

PythonScalarValue::PythonScalarValue(Variable* owner)
  : VariableScalarValue(owner), m_type(Undefined)
{
}

PythonScalarValue::~PythonScalarValue()
{
}

QString PythonScalarValue::typeName()
{
  switch(m_type)
  {
    case Scalar:
      return i18n("Scalar");
    case Object:
      return m_class;
    default:
      return VariableValue::UndefinedType;
  }
}

void PythonScalarValue::setClassName(const QString& className)
{
  m_class = className;
}

void PythonScalarValue::setType(int type)
{
  m_type = type;
}

int PythonScalarValue::type()
{
  return m_type;
}
//----------------------------------------------------------------

PythonListValue::PythonListValue(Variable* owner, int size, int type)
  : VariableListValue(owner), m_size(size), m_type(type)
{
}

PythonListValue::~PythonListValue()
{
}

int PythonListValue::type()
{
  return m_type;
}

int PythonListValue::size()
{
  return m_size;
}

//----------------------------------------------------------------

PythonArrayValue::PythonArrayValue(Variable* owner, int size)
  : PythonListValue(owner, size, PythonListValue::Array)
{
}

PythonArrayValue::~PythonArrayValue()
{
}

QString PythonArrayValue::toString(int indent)
{
  QString ind;
  ind.fill(' ', indent);

  QString s;

  Variable* v;
  for(v =  m_list->first(); v; v = m_list->next()) {
    s += "\n";
    s += ind;

    if(v->isReference()) {
      s += v->name() + " => \\" + v->value()->owner()->name();
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

QString PythonArrayValue::typeName()
{  
  QString s = i18n("Array");
  s += "[" + QString::number(size()) + "]";

  return s;
}

//----------------------------------------------------------------

PythonHashValue::PythonHashValue(Variable* owner, int size)
  : PythonListValue(owner, size, PythonListValue::Hash)
{
}


PythonHashValue::~PythonHashValue()
{
}

QString PythonHashValue::toString(int indent)
{
  QString ind;
  ind.fill(' ', indent);

  QString s;

  Variable* v;
  for(v =  m_list->first(); v; v = m_list->next()) {
    s += "\n";
    s += ind;

    if(v->isReference()) {
      s += v->name() + " => \\" + v->value()->owner()->name();
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

QString PythonHashValue::typeName()
{  
  return i18n("Hash");;
}

