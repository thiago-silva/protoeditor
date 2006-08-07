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

#include "pythondbgpvariableparser.h"

#include <qdom.h>
#include <kmdcodec.h>

PythonDBGPVariableParser::PythonDBGPVariableParser()
{}

PythonDBGPVariableParser::~PythonDBGPVariableParser()
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

VariableList_t* PythonDBGPVariableParser::parse(QDomNodeList& list)
{
  return parseList(list, 0);
}

PythonVariable* PythonDBGPVariableParser::parse(QDomNode& node)
{
  return parseVar(node, 0);
}

VariableList_t* PythonDBGPVariableParser::parseList(const QDomNodeList& list, Variable* parent)
{
  QDomElement e;
  VariableList_t* vlist = new VariableList_t;
  PythonVariable* var;
  for(uint i = 0; i < list.count(); i++)
  {
    e = list.item(i).toElement();
    
    if(e.tagName() != "property")
      continue;

    var = parseVar(e, parent);
    vlist->append(var);
  }
  return vlist;
}

PythonVariable* PythonDBGPVariableParser::parseVar(QDomNode& node, Variable* parent)
{
/*
<?xml version="1.0" encoding="utf-8"?>
	<response xmlns="urn:debugger_protocol_v1" command="context_get" context="0" transaction_id="4">
		<property  type="int" children="0" size="0">
			<value><![CDATA[0]]></value>
			<name encoding="base64"><![CDATA[REJHUEhpZGVDaGlsZHJlbg==]]></name>
			<fullname encoding="base64"><![CDATA[REJHUEhpZGVDaGlsZHJlbg==]]></fullname>
		</property>
		<property  type="module" children="0" size="0">
			<value><![CDATA[<module 'sys' (built-in)>]]></value>
			<name encoding="base64"><![CDATA[c3lz]]></name>
			<fullname encoding="base64"><![CDATA[c3lz]]></fullname>
		</property>
		<property  pagesize="300" numchildren="3" children="1" type="tuple" page="0" size="3">
			<name encoding="base64"><![CDATA[dA==]]></name>
			<fullname encoding="base64"><![CDATA[dA==]]></fullname>
		</property>
		<property  type="str" children="0" size="3">
			<value encoding="base64"><![CDATA[b2xh]]></value>	
			<name encoding="base64"><![CDATA[dGVzdGU=]]></name>
			<fullname encoding="base64"><![CDATA[dGVzdGU=]]>
		</fullname>
	</property>
</response>
*/
  QDomElement e = node.toElement();
  
  PythonVariable* var = new PythonVariable(parent);
  
  int effectiveChildren = e.attributeNode("numchildren").value().toInt();

  QString type = e.attributeNode("type").value();
//   int size = e.attributeNode("size").value().toInt();
 

  QDomNodeList list = e.childNodes();  
//   int broughtChildren = list.count();


  QString name;
  QString tmp, tvalue;
  for(uint i = 0; i < list.count(); i++)
  {
    e = list.item(i).toElement();

    tmp = e.text();
    if(e.hasAttributes() && (e.attributeNode("encoding").value() == QString("base64")))
    {
      tmp = KCodecs::base64Decode(tmp.utf8());
    }

    if(e.tagName() == "fullname")
    {
      name = tmp;      
    } 
    else if(e.tagName() == "value")
    {
      tvalue = tmp;
    }
  }
  
  var->setName(name);

  if(effectiveChildren == 0) //scalar
  {
    PythonScalarValue* value = new PythonScalarValue(var);
    value->setTypeName(type);
    var->setValue(value);
    value->set(tvalue);
  }
  else
  {
    PythonListValue* value = new PythonListValue(var, type);
    value->setScalar(false);
    var->setValue(value);
//     value->setList(parseList(e.childNodes(), var));
  }

/*
  if(type == "array")
  {
    PerlArrayValue* arrayValue = new PerlArrayValue(var, effectiveChildren);
    var->setValue(arrayValue);
    arrayValue->setScalar(false);
    
    if((effectiveChildren == 0) || (broughtChildren != 0))
    {
      arrayValue->setList(parseList(e.childNodes(), var));
    }*/

//   if(type == "hash")
//   {
//     PythonHashValue* hashValue = new PythonHashValue(var, effectiveChildren);
//     var->setValue(hashValue);
//     hashValue->setScalar(false);
// 
//     if((effectiveChildren == 0) || (broughtChildren != 0))
//     {
//       hashValue->setList(parseList(e.childNodes(), var));
//     }
//   }
//   else if(type == "array")
//   {
//     PythonArrayValue* arrayValue = new PythonArrayValue(var, effectiveChildren);
//     var->setValue(arrayValue);
//     arrayValue->setScalar(false);
//     
//     if((effectiveChildren == 0) || (broughtChildren != 0))
//     {
//       arrayValue->setList(parseList(e.childNodes(), var));
//     }
//   }
//   else if(type == "scalar")
//   {
//   }
//   else //Object
//   {
//     PythonScalarValue* value = new PythonScalarValue(var);    
//     value->setType(PythonScalarValue::Object);
//     value->setClassName(type);
//     var->setValue(value);    
//   }

  return var;
}
