#include <iostream>
#include <fstream>
#include "antlr/AST.hpp"
#include "PHPLexer.hpp"
#include "PHPParser.hpp"


bool PHPLexer::php_context = false;

void do_parse(istream& in) {
//   DEBUG_PARSER = 1;
  try {    

    PHPLexer lexer(in);
    PHPParser parser(lexer);
    
		ASTFactory ast_factory;
		parser.initializeASTFactory(ast_factory);
		parser.setASTFactory(&ast_factory);

    parser.start();

    RefAST _astree = parser.getAST();

    if(_astree) {    
      //std::cerr << _astree->toStringList() << std::endl << std::endl;
      std::cerr << _astree->toStringTree() << std::endl << std::endl;
    }
  }
  catch (exception& e) {
		cerr << "parser exception: " << e.what() << endl;
	}
}


int main(int argc, char** argv)
{	
  using namespace std;
	using namespace antlr;

  if(argc != 2) {
    cerr << "no arguments" << endl;
    exit(0);
  }

  ifstream *fin = new ifstream(argv[1]);
  if(!*fin) {
    cerr << "error opening" << argv[1] << std::endl;
    return 1;
  } else {
    do_parse(*fin);
  }
  return 0;
}
