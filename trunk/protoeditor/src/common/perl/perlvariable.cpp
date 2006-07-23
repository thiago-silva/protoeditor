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

#include "perlvariable.h"
#include <klocale.h>

PerlVariable::PerlVariable(Variable* parent)
  : Variable(parent)
{
}
PerlVariable::PerlVariable(QString name)
  : Variable(name)
{
}
PerlVariable::~PerlVariable() {
}

QString PerlVariable::compositeName()
{
  return perlCompositeName(false);
}

QString PerlVariable::perlCompositeName(bool eval)
{
  if(!parent()) 
  {
    return perlName(eval);
  }
  else
  {
    QString cname = dynamic_cast<PerlVariable*>(parent())->perlCompositeName(true);
    return cname + name();
  }
}
  

QString PerlVariable::perlName(bool eval)
{
  if(!eval)
  {
    return name();
  }
  else
  {
    switch(value()->type())
    {
      case PerlListValue::Array:
        return name().replace('@', '$');
      case PerlListValue::Hash:
        return name().replace('%', '$');
      case PerlListValue::Object:
        return name();
    }
  }
  return QString::null;
}

//------------------------------------------------------

PerlScalarValue::PerlScalarValue(Variable* owner)
  : VariableScalarValue(owner), m_type(Undefined)
{
}

PerlScalarValue::~PerlScalarValue()
{
}

QString PerlScalarValue::typeName()
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

void PerlScalarValue::setClassName(const QString& className)
{
  m_class = className;
}

void PerlScalarValue::setType(int type)
{
  m_type = type;
}

int PerlScalarValue::type()
{
  return m_type;
}
//----------------------------------------------------------------

PerlListValue::PerlListValue(Variable* owner, int size, int type)
  : VariableListValue(owner), m_size(size), m_type(type)
{
}

PerlListValue::~PerlListValue()
{
}

QString PerlListValue::toString(int indent)
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

QString PerlListValue::typeName()
{
  QString name;

  switch(type())
  {
    case Array:
      name = i18n("Array");
      name += "[" + QString::number(size()) + "]";
      break;
    case Hash:
      name = i18n("Hash");
      break;
    case Object: 
      name = i18n("Object");
      break;
  }
  return name;
}

int PerlListValue::type()
{
  return m_type;
}

int PerlListValue::size()
{
  return m_size;
}
