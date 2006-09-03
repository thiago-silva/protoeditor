INCLUDES = -I$(top_srcdir)/src/nodeclasses $(all_includes)
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libphpparser_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libphpparser.la
libphpparser_la_LIBADD = -lantlr

BUILT_SOURCES = PHPLexer.cpp  PHPParser.cpp PHPNodeWalker.cpp
ANTLR_BIN = antlr

libphpparser_la_SOURCES = $(BUILT_SOURCES) phpbrowserparser.cpp PHPAST.cpp
noinst_HEADERS = phpbrowserparser.h PHPAST.hpp

lexer_g       = lexer.g
parser_g      = parser.g
walker_g      = walker.g

EXTRA_DIST = $(lexer_g) $(parser_g) $(walker_h)

CLEANFILES = PHPLexer.cpp PHPLexer.hpp PHPParser.cpp PHPParser.hpp \
             PHPNodeWalker.cpp PHPNodeWalker.h PHPParserTokenTypes.hpp \
						 PHPParserTokenTypes.txt \
             PHPTokenTypes.hpp PHPTokenTypes.txt

PHPLexer.cpp: $(srcdir)/$(lexer_g)
	$(ANTLR_BIN) $(srcdir)/$(lexer_g)

PHPParser.cpp: PHPLexer.cpp $(srcdir)/$(parser_g)
	$(ANTLR_BIN) $(srcdir)/$(parser_g)

PHPNodeWalker.cpp: PHPParser.cpp $(srcdir)/$(walker_g)
	$(ANTLR_BIN) $(srcdir)/$(walker_g)
