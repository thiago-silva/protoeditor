INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/commonphp $(all_includes)

METASOURCES = AUTO

noinst_LTLIBRARIES = libcommonphp.la
libcommonphp_la_SOURCES = phpvariable.cpp phpvariableparser.cpp

libcommonphp_la_LDFLAGS = $(all_libraries)
noinst_HEADERS = phpdefs.h phpvariable.h phpvariableparser.h
