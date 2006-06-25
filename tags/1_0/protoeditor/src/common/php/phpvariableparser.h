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


#ifndef DBGVARIABLEPARSER_H
#define DBGVARIABLEPARSER_H


#include <qptrlist.h>
#include <qstring.h>
#include "phpvariable.h"

/*
  Used to parse PHP serialized text into a list of Variables 
*/

class PHPVariableParser{
public:
  PHPVariableParser(const QString& raw);
  ~PHPVariableParser();

  PHPVariable* parseVariable();
  VariablesList_t* parseAnonymousArray();

  PHPVariable* parseVarName(PHPVariable*);

  VariableValue* parseValue(PHPVariable*);

  QString parseString();
  QString parseInt();
  QString parseDouble();
  QString parseBool();
  QString parseResource();

  VariablesList_t* parseArray(PHPVariable*);

  QString parseClassType();
  VariablesList_t* parseObjectMembers(PHPVariable*);

  int parseSoftReference();
  int parseReference();


  //QPtrList<VariableValue> indexedVarList();
  //void setIndexedVarList(QPtrList<VariableValue>);

private:

  QString m_raw;
  long m_index;

  //list with all the variables (no references)
  //used to map the references
  QPtrList<VariableValue> m_indexedVarList; //for references

};

#endif
