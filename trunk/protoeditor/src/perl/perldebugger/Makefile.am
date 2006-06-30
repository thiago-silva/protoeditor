INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/perl -I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/settings \
	-I$(top_srcdir)/src/perl/perldebugger $(all_includes) -I../settings

METASOURCES = AUTO
libperldebugger_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libperldebugger.la
noinst_HEADERS = perldebugger.h perldebuggersettings.h \
	perldebuggersettingswidget.h
libperldebugger_la_SOURCES = perldebugger.cpp perldebuggersettings.cpp \
	perldebuggersettingswidget.cpp

libperldebugger_la_LIBADD = $(top_builddir)/src/settings/libsettings.la \
	$(top_builddir)/src/common/libcommon.la
