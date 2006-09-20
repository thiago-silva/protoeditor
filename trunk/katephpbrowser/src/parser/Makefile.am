INCLUDES     = -I$(top_srcdir)/src/nodeclasses $(all_includes)
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

noinst_LTLIBRARIES = libphpparser.la

BUILT_SOURCES = PHPLexer.cpp  PHPParser.cpp PHPNodeWalker.cpp

nodist_libphpparser_la_SOURCES = PHPLexer.cpp  PHPParser.cpp PHPNodeWalker.cpp

libphpparser_la_SOURCES = phpbrowserparser.cpp PHPAST.cpp

noinst_HEADERS = phpbrowserparser.h PHPAST.hpp

lexer_g       = $(srcdir)/lexer.g
parser_g      = $(srcdir)/parser.g
walker_g      = $(srcdir)/walker.g

EXTRA_DIST = $(lexer_g) $(parser_g) $(walker_g)

CLEANFILES = PHPLexer.cpp PHPLexer.hpp PHPParser.cpp PHPParser.hpp \
             PHPNodeWalker.cpp PHPNodeWalker.hpp PHPParserTokenTypes.hpp \
						 PHPParserTokenTypes.txt PHPNodeWalkerTokenTypes.hpp \
						 PHPNodeWalkerTokenTypes.txt PHPTokenTypes.hpp PHPTokenTypes.txt

PHPLexer.cpp: $(lexer_g)
	$(ANTLR_BIN) $(lexer_g)

PHPParser.cpp: PHPLexer.cpp $(parser_g)
	$(ANTLR_BIN) $(parser_g)

PHPNodeWalker.cpp: PHPParser.cpp $(walker_g)
	$(ANTLR_BIN) $(walker_g)

# #! libphpparser_la_LDFLAGS = $(all_libraries)
# #! libphpparser_la_LIBADD = -lantlr