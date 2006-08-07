INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/perl -I$(top_srcdir)/src/perl -I$(top_srcdir)/src/perl/perldb \
	-I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/settings $(all_includes) -I../settings

METASOURCES = AUTO
libperldb_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libperldb.la
noinst_HEADERS = perldebugger.h perldebuggersettings.h \
	perldebuggersettingswidget.h perlcom.h perllocalcom.h perldebugger.h perldebuggersettings.h \
	perldebuggersettingswidget.h perllocalcom.h
libperldb_la_SOURCES = perldebugger.cpp perldebuggersettings.cpp \
		perldebuggersettingswidget.cpp perllocalcom.cpp


