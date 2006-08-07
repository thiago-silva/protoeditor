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

#include "perldbgpvariableparser.h"

#include <qdom.h>
#include <kmdcodec.h>

PerlDBGPVariableParser::PerlDBGPVariableParser()
{}

PerlDBGPVariableParser::~PerlDBGPVariableParser()
{}

/*
<response xmlns="urn:debugger_protocol_v1" command="context_get"
                         context_id="0"
                         transaction_id="4" ><property fullname="@arr" name="@arr" type="array" constant="0" children="1" numchildren="3" address="865900c" size="0" page="0" pagesize="10" ></property>
<property fullname="%has" name="%has" type="hash" constant="0" children="1" numchildren="1" address="869d054" size="0" page="0" pagesize="10" ></property>
<property fullname="$scal" name="$scal" encoding="base64" type="scalar" constant="0" children="0" size="9" ><value encoding="base64"><![CDATA[dGhpYWdv
]]></value>
</property>
<property fullname="$server" name="$server" encoding="base64" type="IO::Socket::INET" constant="0" children="0" size="45" ></property>

</response>


*/

VariableList_t* PerlDBGPVariableParser::parse(QDomNodeList& list)
{
  return parseList(list, 0);
}

PerlVariable* PerlDBGPVariableParser::parse(QDomNode& node)
{
  return parseVar(node, 0);
}

VariableList_t* PerlDBGPVariableParser::parseList(const QDomNodeList& list, Variable* parent)
{
  QDomElement e;
  VariableList_t* vlist = new VariableList_t;
  PerlVariable* var;
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

PerlVariable* PerlDBGPVariableParser::parseVar(QDomNode& node, Variable* parent)
{
  QDomElement e = node.toElement();
  
  PerlVariable* var = new PerlVariable(parent);

  int effectiveChildren = e.attributeNode("numchildren").value().toInt();
  int broughtChildren = e.childNodes().count();

  QString type = e.attributeNode("type").value();
  
  var->setName(e.attributeNode("name").value());

  if(type == "hash")
  {
    PerlListValue* hashValue = new PerlListValue(var, effectiveChildren, PerlListValue::Hash);
    var->setValue(hashValue);
    hashValue->setScalar(false);

    if((effectiveChildren == 0) || (broughtChildren != 0))
    {
      hashValue->setList(parseList(e.childNodes(), var));
    }
  }
  else if(type == "array")
  {
    PerlListValue* arrayValue = new PerlListValue(var, effectiveChildren, PerlListValue::Array);
    var->setValue(arrayValue);
    arrayValue->setScalar(false);
    
    if((effectiveChildren == 0) || (broughtChildren != 0))
    {
      arrayValue->setList(parseList(e.childNodes(), var));
    }
  }
  else if(type == "object")
  {
    PerlListValue* lvalue = new PerlListValue(var, effectiveChildren, PerlListValue::Object);
    var->setValue(lvalue);
    lvalue->setScalar(false);
    
    if((effectiveChildren == 0) || (broughtChildren != 0))
    {
      lvalue->setList(parseList(e.childNodes(), var));
    }
  }
  else if(type == "scalar")
  {
    PerlScalarValue* value = new PerlScalarValue(var);
    value->setType(PerlScalarValue::Scalar);    
    var->setValue(value);
    value->set(QString("\"") + QString(KCodecs::base64Decode (e.text().utf8())) + QString("\""));
  }
  else //Object
  {
    PerlScalarValue* value = new PerlScalarValue(var);    
    value->setType(PerlScalarValue::Object);
    value->setClassName(type);
    var->setValue(value);    
  }

  return var;
}
