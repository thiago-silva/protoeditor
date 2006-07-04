INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/perl -I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/settings \
	-I$(top_srcdir)/src/perl/perldbgp $(all_includes) -I../settings

METASOURCES = AUTO
libperldbgp_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libperldbgp.la
libperldbgp_la_SOURCES = debuggerperldbgp.cpp perldbgpnet.cpp perldbgpsettings.cpp \
	perldbgpsettingswidget.cpp perldbgpvariableparser.cpp
noinst_HEADERS = debuggerperldbgp.h perldbgpnet.h perldbgpsettings.h\
	perldbgpsettingswidget.h perldbgpvariableparser.h
libperldbgp_la_LIBADD = $(top_builddir)/src/settings/libsettings.la \
	$(top_builddir)/src/common/libcommon.la $(top_builddir)/src/common/perl/libcommonperl.la
