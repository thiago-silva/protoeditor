INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/python -I$(top_srcdir)/src/python/pythondbgp -I$(top_srcdir)/src/protoeditor \
	-I$(top_srcdir)/src/settings $(all_includes) -I../settings

METASOURCES = AUTO

AM_CXXFLAGS = -DPYTHONDBGPDIR="\"$(pythonlibdir)\""

libpythondbgp_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libpythondbgp.la
libpythondbgp_la_SOURCES = debuggerpythondbgp.cpp pythondbgpnet.cpp pythondbgpsettings.cpp \
	pythondbgpsettingswidget.cpp pythondbgpvariableparser.cpp
noinst_HEADERS = debuggerpythondbgp.h pythondbgpnet.h pythondbgpsettings.h\
	pythondbgpsettingswidget.h pythondbgpvariableparser.h
libpythondbgp_la_LIBADD = $(top_builddir)/src/settings/libsettings.la \
	$(top_builddir)/src/common/libcommon.la $(top_builddir)/src/common/python/libcommonpython.la

