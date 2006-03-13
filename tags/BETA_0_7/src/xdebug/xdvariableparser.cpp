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

#include "xdvariableparser.h"
#include "phpvariable.h"

#include <qdom.h>
#include <kmdcodec.h>

XDVariableParser::XDVariableParser()
{}

XDVariableParser::~XDVariableParser()
{}

/*
<response command="context_get" transaction_id="1">
 <property name="obj" fullname="$obj" address="135610740" type="object" children="1" classname="Teste" numchildren="1">
  <property name="a" fullname="$obj-&gt;a" facet="protected" address="135586308" type="null">
  </property>
 </property>
 <property name="arr" fullname="$arr" address="135581636" type="hash" children="1" numchildren="3">
  <property name="0" fullname="$arr[0]" address="135574012" type="int">
   <![CDATA[99]]>
  </property>
  <property name="1" fullname="$arr[1]" address="135586860" type="int">
   <![CDATA[98]]>
  </property>
  <property name="2" fullname="$arr[2]" address="135587172" type="int">
   <![CDATA[97]]>
  </property>
 </property>
 <property name="str" fullname="$str" address="135586948" type="string" encoding="base64">
  <![CDATA[YQ==]]>
 </property>
 <property name="num" fullname="$num" address="135586916" type="int">
  <![CDATA[1]]>
 </property>
 <property name="mat" fullname="$mat" address="135586828" type="float">
  <![CDATA[2.4]]>
 </property>
</response>

$GLOBALS
$_ENV
$HTTP_ENV_VARS
$_POST
$HTTP_POST_VARS
$_GET
$HTTP_GET_VARS
$_COOKIE
$HTTP_COOKIE_VARS
$_SERVER
$HTTP_SERVER_VARS
$_FILES
$HTTP_POST_FILES
$_REQUEST

*/

VariablesList_t* XDVariableParser::parse(QDomNodeList& list)
{
  return parseList(list, 0);
}

PHPVariable* XDVariableParser::parse(QDomNode& node)
{
  return parseVar(node, 0);
}

VariablesList_t* XDVariableParser::parseList(const QDomNodeList& list, PHPVariable* parent)
{
  QDomElement e;
  VariablesList_t* vlist = new VariablesList_t;
  PHPVariable* var;
  for(uint i = 0; i < list.count(); i++)
  {
    e = list.item(i).toElement();
    
    if(e.attributeNode("type").value() == "uninitialized")
      continue;

    var = parseVar(e, parent);
    vlist->append(var);
  }
  return vlist;
}

PHPVariable* XDVariableParser::parseVar(QDomNode& node, PHPVariable* parent)
{
  QDomElement e = node.toElement();
  
  PHPVariable* var = new PHPVariable(parent);

  int children = e.attributeNode("numchildren").value().toInt();
  QString type = e.attributeNode("type").value();
  
  var->setName(e.attributeNode("name").value());

  if(type == "object")
  {
    PHPObjectValue* objValue = new PHPObjectValue(var);
    var->setValue(objValue);
    objValue->setScalar(false);
    objValue->setClassType(e.attributeNode("classname").value());
    objValue->setList(parseList(e.childNodes(), var));
  }
  else if(children)
  {
    PHPArrayValue* arrayValue = new PHPArrayValue(var);
    var->setValue(arrayValue);
    arrayValue->setScalar(false);
    arrayValue->setList(parseList(e.childNodes(), var));
  }
  else
  {
    PHPScalarValue* value = new PHPScalarValue(var);
    var->setValue(value);

    if(type == "null")
    {
      value->setType(PHPScalarValue::Undefined);
      value->set("null");
    }
    else if(type == "int")
    {
      value->setType(PHPScalarValue::Integer);
      value->set(e.text());
    }
    else if(type == "string")
    {
      value->setType(PHPScalarValue::String);
      value->set(QString("\"") + QString(KCodecs::base64Decode (e.text().utf8())) + QString("\""));
    }
    else if(type == "float")
    {
      value->setType(PHPScalarValue::Double);
      value->set(e.text());
    }
  }
  return var;
}
