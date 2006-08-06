INCLUDES = $(all_includes)
bin_PROGRAMS = test
test_LDFLAGS = $(all_libraries)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
# test_LIBADD = -lantlr
test_LDADD = -lantlr

BUILT_SOURCES = PHPLexer.cpp  PHPParser.cpp
ANTLR_BIN = antlr

test_SOURCES = main.cpp $(BUILT_SOURCES)

lexer_g       = lexer.g
parser_g      = parser.g

EXTRA_DIST = $(lexer_g) $(parser_g)

CLEANFILES = PHPLexer.cpp PHPLexer.hpp PHPParser.cpp PHPParser.hpp \
             PHPParserTokenTypes.hpp PHPParserTokenTypes.txt \
             PHPTokenTypes.hpp PHPTokenTypes.txt

PHPLexer.cpp: $(srcdir)/$(lexer_g)	
	$(ANTLR_BIN) $(srcdir)/$(lexer_g)

PHPParser.cpp: $(srcdir)/$(parser_g) PHPLexer.cpp
	$(ANTLR_BIN) $(srcdir)/$(parser_g)

