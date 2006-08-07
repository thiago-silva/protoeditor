INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/perl -I$(top_srcdir)/src/perl/perldbgp -I$(top_srcdir)/src/protoeditor \
	-I$(top_srcdir)/src/settings $(all_includes) -I../settings

METASOURCES = AUTO

AM_CXXFLAGS = -DPERLDBGPDIR="\"$(perllibdir)\""

libperldbgp_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libperldbgp.la
libperldbgp_la_SOURCES = debuggerperldbgp.cpp perldbgpnet.cpp perldbgpsettings.cpp \
	perldbgpsettingswidget.cpp perldbgpvariableparser.cpp
noinst_HEADERS = debuggerperldbgp.h perldbgpnet.h perldbgpsettings.h\
	perldbgpsettingswidget.h perldbgpvariableparser.h


