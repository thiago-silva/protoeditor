INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/protoeditor $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libcommon.la
libcommon_la_SOURCES = abstractdebugger.cpp debuggerbreakpoint.cpp variable.cpp \
	debuggerstack.cpp
noinst_HEADERS = abstractdebugger.h debuggerbreakpoint.h variable.h \
	debuggerstack.h

libcommon_la_LDFLAGS = $(all_libraries)
