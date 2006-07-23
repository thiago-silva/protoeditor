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

#ifndef PerlVARIABLE_H
#define PerlVARIABLE_H

#include "variable.h"


class PerlVariable : public Variable {
public:
  PerlVariable(Variable* parent);
  PerlVariable(QString name);
  virtual ~PerlVariable();

  virtual QString compositeName();  /* ie. house[type], where:
                                         type is this->name()
                                         house is this->parent->name()
                                     */
protected:
  QString perlCompositeName(bool);
  QString perlName(bool);
};

class PerlScalarValue : public VariableScalarValue {
public:
  enum { Scalar = 1, Object };

  PerlScalarValue(Variable* owner);
  virtual ~PerlScalarValue();

  virtual void setType(int);
  virtual int type();

  void setClassName(const QString&);

  virtual QString typeName();
private:
  int m_type;
  QString m_class;
};

//---------------------------------------------------------------

class PerlListValue : public VariableListValue
{
public:
  enum { Array = 1, Hash, Object };

  PerlListValue(Variable* owner, int size, int type);
  virtual ~PerlListValue();  

  virtual int     type();
protected:
  int size();
private:
  int m_size;
  int m_type;
};

class PerlArrayValue : public PerlListValue {
public:
  PerlArrayValue(Variable* owner, int size);
  virtual ~PerlArrayValue();

  virtual QString toString(int indent = 0);
  virtual QString typeName();
};

class PerlHashValue : public PerlListValue {
public:
  PerlHashValue(Variable* owner, int size);
  virtual ~PerlHashValue();

  virtual QString toString(int indent = 0);
  virtual QString typeName();
};


#endif
