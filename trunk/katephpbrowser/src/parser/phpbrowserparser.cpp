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
#include "phpbrowserparser.h"

#include <sstream>

#include <kurl.h>

#include "PHPLexer.hpp"
#include "PHPParser.hpp"
#include "PHPNodeWalker.hpp"

#include <qfile.h>
#include <qptrlist.h>

PHPBrowserParser::PHPBrowserParser()
{
}


PHPBrowserParser::~PHPBrowserParser()
{
}

BrowserNode* PHPBrowserParser::parseURL(const KURL& url)
{
  QFile file(url.path());
  file.open(IO_ReadOnly);

  stringstream s;
  s << QString(file.readAll()).latin1();

//   DEBUG_PARSER = true;
  RefPHPAST astree;

  PHPBrowserParser parser;
  
  BrowserNode* root = new BrowserNode();
  root->setFileURL(url);
  root->setName(url.filename());

  try 
  {
    QPtrList<BrowserNode> list;

    PHPLexer lexer(s);
    PHPParser parser(lexer);
    
		antlr::ASTFactory ast_factory(PHPAST::TYPE_NAME,&PHPAST::factory);
		parser.initializeASTFactory(ast_factory);
		parser.setASTFactory(&ast_factory);

    parser.start();

    astree = parser.getAST();

    if(astree)
    {
//       std::cerr << "\n\n" << astree->toStringTree() << std::endl << std::endl;
  
      PHPNodeWalker walker(url);
      list = walker.start(astree);      
      root->setChilds(list);
    }
  }
  catch (exception& e) 
  { 
    root->setError(true);
  }
  
  return root;
}
