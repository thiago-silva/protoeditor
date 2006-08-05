INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/perl -I$(top_srcdir)/src/perl -I$(top_srcdir)/src/perl/perldb \
	-I$(top_srcdir)/src/perl/perldbgp -I$(top_srcdir)/src/settings $(all_includes)

METASOURCES = AUTO
SUBDIRS = perldbgp
libperllang_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libperllang.la
libperllang_la_SOURCES = perlsettings.cpp perlsettingswidget.cpp

noinst_HEADERS = perlsettings.h perlsettingswidget.h

