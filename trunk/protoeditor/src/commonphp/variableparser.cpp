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


#include "variableparser.h"

#include <qregexp.h>
#include <kdebug.h>
#include "phpvariable.h"

VariableParser::VariableParser(const QString& raw)
  : m_raw(raw), m_index(0)
{
}

VariableParser::~VariableParser()
{
}

PHPVariable* VariableParser::parseVariable() {
  PHPVariable* var = new PHPVariable(NULL);
  VariableValue* value = parseValue(var);
  var->setValue(value);
  return var;
}

VariablesList_t* VariableParser::parseAnonymousArray() {
  //pretend that there is the first variable so the references
  //are set correctly
  m_indexedVarList.append(NULL);
  return parseArray(NULL);
}


VariablesList_t* VariableParser::parseArray(PHPVariable* var)
{
  int size;
  VariablesList_t* list = new VariablesList_t;

  QRegExp rx;
  rx.setPattern("a:(\\d*):\\{");
  if(rx.search(m_raw, m_index) == -1) return list;

  size    =  rx.cap(1).toInt();
  m_index += rx.matchedLength();

  for(int i = 0; i < size; i++) {
    list->append(parseVarName(var));
  }

  m_index++; //eats the '}'
  return list;
}

PHPVariable* VariableParser::parseVarName(PHPVariable* parent)
{
  QString name;
  switch(m_raw.at(m_index).latin1())
  {
    case 'i':
      name = parseInt();
      break;
    case 's':
      name = parseString();
      break;
  }

  PHPVariable* var = new PHPVariable(parent);
  var->setName(name);

  VariableValue* value = parseValue(var);
  var->setValue(value);
  return var;
}

QString VariableParser::parseString()
{
  QRegExp rx;
  rx.setPattern("s:(\\d*):");

  if(rx.search(m_raw, m_index) == -1) return "";
  m_index += rx.matchedLength()+1; //\"

  long strlen = rx.cap(1).toLong();

  QString r = m_raw.mid(m_index, strlen);

  m_index += strlen+2; //\";

  return r;
  /*
  QRegExp rx;
  rx.setPattern("s:\\d*:\"([^\"]*)\";");

  if(rx.search(m_raw, m_index) == -1) return "";
  m_index += rx.matchedLength();

  return rx.cap(1);
  */
}

QString VariableParser::parseInt()
{
  QRegExp rx;
  rx.setPattern("i:(\\d*);");

  if(rx.search(m_raw, m_index) == -1) return "";
  m_index += rx.matchedLength();

  return rx.cap(1);
}

QString VariableParser::parseDouble()
{
  QRegExp rx;
  rx.setPattern("d:([^;]*);"); //ie. d:1.11111111111E+18;

  if(rx.search(m_raw, m_index) == -1) return "";
  m_index += rx.matchedLength();

  return rx.cap(1);
}

QString VariableParser::parseBool()
{
  QRegExp rx;
  rx.setPattern("b:(\\d*);");

  if(rx.search(m_raw, m_index) == -1) return "";
  m_index += rx.matchedLength();
  if(rx.cap(1) == "1") {
    return QString("true");
  } else {
    return QString("false");
  }
}

QString VariableParser::parseResource()
{
  //z:6:\"stream\":3;
  QRegExp rx;
  rx.setPattern("z:\\d*:[^:]*:(\\d*);");

  if(rx.search(m_raw, m_index) == -1) return "";
  m_index += rx.matchedLength();

  return rx.cap(1);
}

QString VariableParser::parseClassType()
{
  //O:6:"Classe":3:{s:6:...
  QRegExp rx;
  rx.setPattern("O:\\d*:\"([^\"]*)\":");

  if(rx.search(m_raw, m_index) == -1) return "";
  m_index += rx.matchedLength();

  return rx.cap(1);
}

VariablesList_t* VariableParser::parseObjectMembers(PHPVariable* parent)
{
  //O:6:"Classe":3:{s:6:"membro";N;s:4:"mem2";N;s:4:"priv";N;}
  int size;
  VariablesList_t* list = new VariablesList_t;

  QRegExp rx;
  rx.setPattern("(\\d*):\\{");
  if(rx.search(m_raw, m_index) == -1) return list;

  size    =  rx.cap(1).toInt();
  m_index += rx.matchedLength();

  for(int i = 0; i < size; i++) {
    list->append(parseVarName(parent));
  }

  m_index++; //eats the '}'
  return list;
}

VariableValue* VariableParser::parseValue(PHPVariable* var)
{
  PHPArrayValue* arrayValue;
  PHPObjectValue* objectValue;
  PHPScalarValue* scalarValue;
  int index;

  switch(m_raw.at(m_index).latin1())
  {
    case 'b':
      scalarValue = new PHPScalarValue(var);
      scalarValue->setType(PHPScalarValue::Boolean);

      //var->setValue(scalarValue);
      m_indexedVarList.append(scalarValue);
      scalarValue->set(parseBool());
      return scalarValue;
      //break;
    case 's':
      scalarValue = new PHPScalarValue(var);
      scalarValue->setType(PHPScalarValue::String);
      //var->setValue(scalarValue);
      m_indexedVarList.append(scalarValue);
      scalarValue->set(QString("\"") + parseString() + QString("\""));
      return scalarValue;
      //break;
    case 'a':
      arrayValue = new PHPArrayValue(var);
      arrayValue->setScalar(false);
      //var->setValue(arrayValue);
      m_indexedVarList.append(arrayValue);
      arrayValue->setList(parseArray(var));
      return arrayValue;
      //break;
    case 'O':
      objectValue = new PHPObjectValue(var);
      objectValue->setScalar(false);
      //var->setValue(objectValue);
      m_indexedVarList.append(objectValue);
      objectValue->setClassType(parseClassType());
      objectValue->setList(parseObjectMembers(var));
      return objectValue;
      //break;
    case 'i':
      scalarValue = new PHPScalarValue(var);
      //var->setValue(scalarValue);
      m_indexedVarList.append(scalarValue);

      scalarValue->setType(PHPScalarValue::Integer);
      scalarValue->set(parseInt());
      return scalarValue;
      //break;
    case 'r':
      index = parseSoftReference();
      var->setValueShared(true);
      //var->setValue(m_indexedVarList.at(index-2));
      return m_indexedVarList.at(index-1);
      //break;
    case 'R':
      index = parseReference();
      var->setValueShared(true);
      var->setReference(true);
      return m_indexedVarList.at(index-1);
      //var->setValue(m_indexedVarList.at(index-2));
      break;
    case 'N':
      scalarValue = new PHPScalarValue(var);
      //var->setValue(scalarValue);
      m_indexedVarList.append(scalarValue);

      scalarValue->setType(PHPScalarValue::Undefined);
      scalarValue->set("null");
      m_index+=2; //eats the 'N;'
      return scalarValue;
      //break;
    case 'd':
    scalarValue = new PHPScalarValue(var);
      //var->setValue(scalarValue);
      m_indexedVarList.append(scalarValue);

      scalarValue->setType(PHPScalarValue::Double);
      scalarValue->set(parseDouble());
      return scalarValue;
    case 'z':
      scalarValue = new PHPScalarValue(var);
      m_indexedVarList.append(scalarValue);
      scalarValue->setType(PHPScalarValue::Resource);
      scalarValue->set(parseResource());
      return scalarValue;
    default:
      return 0;
  }
}

int VariableParser::parseReference() {
  QRegExp rx;
  rx.setPattern("R:(\\d*);");
  if(rx.search(m_raw, m_index) == -1) return 0;

  m_index += rx.matchedLength();

  return rx.cap(1).toInt();
}

int VariableParser::parseSoftReference()
{
  QRegExp rx;
  rx.setPattern("r:(\\d*);");
  if(rx.search(m_raw, m_index) == -1) return 0;

  m_index += rx.matchedLength();

  return rx.cap(1).toInt();
}

/*
QPtrList<VariableValue> VariableParser::indexedVarList() {
  return m_indexedVarList;
}

void VariableParser::setIndexedVarList(QPtrList<VariableValue> indexedVarList) {
  m_indexedVarList = indexedVarList;
}
*/

