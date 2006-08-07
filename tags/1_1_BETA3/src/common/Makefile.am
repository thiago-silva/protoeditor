SUBDIRS = . php perl python

INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/settings $(all_includes)

METASOURCES = AUTO

noinst_LTLIBRARIES = libcommon.la
libcommon_la_SOURCES = abstractdebugger.cpp debuggerbreakpoint.cpp variable.cpp \
			debuggerstack.cpp connection.cpp session.cpp
noinst_HEADERS = abstractdebugger.h debuggerbreakpoint.h variable.h \
			debuggerstack.h connection.h session.h

libcommon_la_LDFLAGS = $(all_libraries)

