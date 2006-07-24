INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/php -I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/settings \
	-I$(top_srcdir)/src/php/xdebug $(all_includes) -I../settings
METASOURCES = AUTO
libxdebug_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libxdebug.la
noinst_HEADERS = xdsettings.h xdsettingswidget.h debuggerxd.h xdnet.h \
	xdvariableparser.h
libxdebug_la_SOURCES = xdsettings.cpp xdsettingswidget.cpp debuggerxd.cpp \
		xdnet.cpp xdvariableparser.cpp
libxdebug_la_LIBADD = $(top_builddir)/src/settings/libsettings.la \
	$(top_builddir)/src/common/libcommon.la
