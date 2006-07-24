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
  QString cname;
//   if(parent()) {
//     cname = parent()->compositeName();
//     switch(parent()->value()->type())
//     {
//       case PythonListValue::Dictionary:
//         cname += "{'" + name() + "'}";  
//     if(parent()->value()->type() == ) {
//       
//     } else {
//       cname += "->" + name();
//     }
//   } else {
//     cname = name();
//   }

  return name();
}


//------------------------------------------------------

PythonScalarValue::PythonScalarValue(Variable* owner)
  : VariableScalarValue(owner), m_typeName(VariableValue::UndefinedType)
{
}

PythonScalarValue::~PythonScalarValue()
{
}

void PythonScalarValue::setTypeName(const QString& typeName)
{
  m_typeName = typeName;
}

QString PythonScalarValue::typeName()
{
  return m_typeName;
}

int PythonScalarValue::type()
{
  return VariableValue::Undefined;
}

//----------------------------------------------------------------

PythonListValue::PythonListValue(Variable* owner, const QString& typeName)
  : VariableListValue(owner), m_typeName(typeName), m_type(Undefined)
{
  if(m_typeName == "tuple")
  {
    m_type = Tuple;
  } 
  else if(m_typeName == "list")
  {
    m_type = List;
  } 
  else if(m_typeName == "dict")
  {
    m_type = Dictionary;
  }
}

PythonListValue::~PythonListValue()
{
}


QString PythonListValue::typeName()
{  
  return m_typeName;
}

int PythonListValue::type()
{
  return m_type;
}


QString PythonListValue::toString(int indent)
{
  QString ind;
  ind.fill(' ', indent);

  QString s;

//   Variable* v;
//   for(v =  m_list->first(); v; v = m_list->next()) {
//     s += "\n";
//     s += ind;
// 
//     if(v->isReference()) {
//       s += v->name() + " => &" + v->value()->owner()->name();
//     } else {
//       if(v->value()->isScalar()) {
//         s += v->name() + " => " + v->value()->toString();
//       } else {
//         s += v->name() + " = (" + v->value()->typeName() + ")";
//         s += v->value()->toString(indent+3);
//       }
//     }
//   }

  return s;
}
