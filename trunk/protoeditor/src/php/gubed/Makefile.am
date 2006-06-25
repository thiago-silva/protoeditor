INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/php -I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/settings \
	-I$(top_srcdir)/src/gubed -I../settings $(all_includes)
METASOURCES = AUTO
libgubed_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libgubed.la
noinst_HEADERS = gbsettings.h gb_defs.h gbsettingswidget.h gbnet.h debuggergb.h
libgubed_la_SOURCES = gbsettings.cpp gbsettingswidget.cpp gbnet.cpp \
	debuggergb.cpp
libgubed_la_LIBADD = $(top_builddir)/src/settings/libsettings.la\
	$(top_builddir)/src/common/libcommon.la
