INCLUDES = -I$(top_srcdir)/src/common -I$(top_srcdir)/src/php/dbg \
	-I$(top_srcdir)/src/php/gubed -I$(top_srcdir)/src/php/xdebug -I$(top_srcdir)/src/settings $(all_includes)

METASOURCES = AUTO
SUBDIRS = dbg gubed xdebug
libphplang_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libphplang.la
libphplang_la_SOURCES = phpsettings.cpp phpsettingswidget.cpp
noinst_HEADERS = phpsettings.h phpsettingswidget.h
libphplang_la_LIBADD = $(top_builddir)/src/php/xdebug/libxdebug.la \
	$(top_builddir)/src/php/gubed/libgubed.la $(top_builddir)/src/php/dbg/libdbg.la \
	$(top_builddir)/src/settings/libsettings.la
