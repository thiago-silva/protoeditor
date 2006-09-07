/*
 *   Copyright (C) 2006 by Thiago Silva                                    *
 *   thiago.silva@kdemal.net                                               *
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
 *                                                                         */


header {
  using namespace antlr;
  using namespace std;
}

options {
  language="Cpp";  
}

class PHPLexer extends Lexer;
options {
  k=2;
  charVocabulary='\u0003'..'\u00FF'; // latin-1  
  exportVocab=PHP;
  filter=T_IGNORE;
  genHashLines=false;//no #line in the generated code
  testLiterals = true;
  noConstructors=true;
  defaultErrorHandler=false;
}

tokens {
  T_FUNCTION="function";
  T_CLASS="class";
  T_INTERFACE="interface";
  T_PUBLIC="public";
  T_PROTECTED="protected";
  T_PRIVATE="private";
  T_STATIC="static";
  T_VAR="var";
  T_CONST="const";
  T_EXTENDS="extends";
  T_IMPLEMENTS="implements";  
  T_ABSTRACT="abstract";

  T_PHP_MODULE;

  T_VARIABLES;
  T_MODIFIERS;
/*
  T_STATIC_FUNC;
  */
}

{
  public:
    PHPLexer(std::istream& in)
      :  antlr::CharScanner(new antlr::CharBuffer(in),true), php_context(false)
    {
      initLiterals();
    }

  private:
    bool php_context;
}

//////////------------------------

T_TERMINATOR
  : ';'
  {
    if(!php_context) {
      $setType(antlr::Token::SKIP);
    }
  }

  ;

T_OPEN_PAR
  : '('
  {
    if(!php_context) {
      $setType(antlr::Token::SKIP);
    }
  }

  ;

T_CLOSE_PAR
  : ')'
  {
    if(!php_context) {
      $setType(antlr::Token::SKIP);
    }
  }

  ;

protected
T_DIGIT
  : '0'..'9'
  ;

protected
T_LETTER
  : 'a'..'z'|'A'..'Z'
  ;

T_IDENTIFIER
options {
  paraphrase = "T_IDENTIFIER";
  testLiterals = false;
}
  : (T_LETTER | '_') (T_LETTER | T_DIGIT | '_')*
  {
    if(!php_context) {
      $setType(antlr::Token::SKIP);
    }
  }
  ;

T_VARIABLE
options {
  paraphrase = "T_VARIABLE";
}
  : '$'! T_IDENTIFIER
  {
    if(!php_context) {
      $setType(antlr::Token::SKIP);
    }
  }
  ;

T_START_PHP
  :
    (
      '<' (
              '?' ( ('=') | ( "php") )?
            | "script" (T_WS_)+ "language" (T_WS_)* '=' (T_WS_)* ("php"|"\"php\""|"\'php\'") (T_WS_)* '>'
            | '%' ('=')?
          )
    )
  { 
    if(php_context) {
      $setType(antlr::Token::SKIP);
      throw exception();
    }
    $setText("T_START_PHP");
    php_context = true; 
  }
  ;


T_END_PHP
options {
  paraphrase = "T_END_PHP";
}
  : (
      ("?>" | "</script" (T_WS_)* '>') (T_WS_)?
      | "%>" (T_WS_)?
    )
  {
    if(!php_context) {
      $setType(antlr::Token::SKIP);
    }
    php_context = false;
  }
  ;

T_OPEN_BRACKET 
  : '{'
  {
    if(!php_context) {
      $setType(antlr::Token::SKIP);
    }
  }
  ;

T_CLOSE_BRACKET
  : '}'
  { 
    if(!php_context) {
      $setType(antlr::Token::SKIP);
    }
  }

  ;

T_STRING_LIT
  : '"' ( ~( '"' | '\\' | '\n' | '\r' ) | LINE_BRK | ESC)* '"'  
  {
//     if(!php_context) {
      $setType(antlr::Token::SKIP);
//     }
  }

  ;

T_CARAC_LIT
  : '\'' ( ~( '\'' | '\\' | '\n' | '\r' ) | LINE_BRK | ESC)* '\''  
  {
//     if(!php_context) {
      $setType(antlr::Token::SKIP);
//     }
  }

  ;

// T_CARAC_LIT
//   : '\''! ( ~( '\'' | '\\' ) | ESC )? '\''!
//   {$setType(antlr::Token::SKIP);}
//   ;

protected
ESC
  : '\\' ESCAPED
  ;

protected
ESCAPED
  : ('\n') => '\n' {newline();}
  | .
  ;

T_WS_ : (' '
  | '\t'
  | '\n' { newline(); }
  | '\r')
    { $setType(antlr::Token::SKIP); }
  ;

SL_COMMENT
  : "//" (~('\n'))* ('\n')?
    { 
      newline();
      $setType(antlr::Token::SKIP);
    }
  ;

ML_COMMENT
{int line = getLine();}
  : "/*" 
    ( 
      options { generateAmbigWarnings=false; } :  
        '\n'                     {newline();}
      | ('\r' '\n')=> '\r' '\n'  {newline();}
      | '\r'                     {newline();}
      |~('*'|'\n'|'\r')
      | ('*' ~'/' )=> '*' 
    )* 
    "*/"
    {$setType(antlr::Token::SKIP);}
  ;

protected
LINE_BRK
  : '\n'                     {newline();}
  | ('\r' '\n')=> '\r' '\n'  {newline();}
  | '\r'                     {newline();}  
  ;

protected
T_IGNORE
  : ('\n')=> '\n'            {newline();$setType(antlr::Token::SKIP);}
  | ('\r' '\n')=> '\r' '\n'  {newline();$setType(antlr::Token::SKIP);}
  | ('\r')=> '\r'            {newline();$setType(antlr::Token::SKIP);}
  | . {$setType(antlr::Token::SKIP);}
  ;