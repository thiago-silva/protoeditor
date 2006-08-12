/***************************************************************************
 *   Copyright (C) 2006 by Thiago Silva                                    *
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

#ifndef PHPAST_HPP
#define PHPAST_HPP

#include <antlr/CommonAST.hpp>
#include <string>

using namespace std;
using namespace antlr;

class PHPAST : public CommonAST {
public:
  PHPAST();
  PHPAST( RefToken t );
  PHPAST( const CommonAST& other );
  PHPAST( const PHPAST& other );

  ~PHPAST();

  void setLine(int line);
  int getLine();

  virtual RefAST clone() const;

  virtual void initialize(RefToken);
 
  virtual const char* typeName() const;

  static RefAST factory();
  static const char* const TYPE_NAME;
protected:
  int    m_line;
};

typedef ASTRefCount<PHPAST> RefPHPAST;

#endif
