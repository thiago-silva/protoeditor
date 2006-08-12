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

#include "PHPAST.hpp"

const char* const PHPAST::TYPE_NAME = "PHPAST";

PHPAST::PHPAST()
    : CommonAST(), m_line(-1)
{
}

PHPAST::PHPAST(RefToken t)
    : CommonAST(t), m_line(t->getLine())
{
}

PHPAST::PHPAST(const CommonAST& other)
    : CommonAST(other), m_line(-1)
{
}

PHPAST::PHPAST(const PHPAST& other)
    : CommonAST(other), m_line(other.m_line)
{
}

PHPAST::~PHPAST()
{}

RefAST PHPAST::clone() const
{
  PHPAST *ast = new PHPAST(*this);
  return RefAST(ast);
}

const char* PHPAST::typeName() const
{
  return PHPAST::TYPE_NAME;
}

void PHPAST::initialize(RefToken t)
{
  CommonAST::initialize(t);
  setLine(t->getLine());
}

void PHPAST::setLine(int line) {
  m_line = line;
}

int PHPAST::getLine() {
  return m_line;
}

RefAST PHPAST::factory()
{
  return RefAST(new PHPAST);
}
