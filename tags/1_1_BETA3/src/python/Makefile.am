INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/python -I$(top_srcdir)/src/python \
	-I$(top_srcdir)/src/python/pythondbgp -I$(top_srcdir)/src/settings $(all_includes)

METASOURCES = AUTO
SUBDIRS = pythondbgp
libpythonlang_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libpythonlang.la
libpythonlang_la_SOURCES = pythonsettings.cpp pythonsettingswidget.cpp

noinst_HEADERS = pythonsettings.h pythonsettingswidget.h

