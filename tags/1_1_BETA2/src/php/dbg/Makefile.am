INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/common/php -I$(top_srcdir)/src/php/dbg -I$(top_srcdir)/src/php/dbg/profiler \
	-I$(top_srcdir)/src/php/gubed -I$(top_srcdir)/src/php/xdebug -I$(top_srcdir)/src/protoeditor \
	-I$(top_srcdir)/src/settings $(all_includes)
METASOURCES = AUTO



libdbg_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libdbg.la

libdbg_la_SOURCES = dbgnet.cpp dbgnetdata.cpp dbgreceiver.cpp dbgrequestor.cpp \
	dbgrequestpack.cpp dbgrequestpackbuilder.cpp dbgresponsepack.cpp dbgstack.cpp \
	debuggerdbg.cpp dbgfileinfo.cpp dbgsettingswidget.cpp dbgsettings.cpp



SUBDIRS = profiler
libdbg_la_LIBADD = $(top_builddir)/src/common/libcommon.la \
	$(top_builddir)/src/common/php/libcommonphp.la $(top_builddir)/src/php/dbg/profiler/libdbgprofiler.la
noinst_HEADERS = dbg_defs.h dbgfileinfo.h dbgnet.h dbgnetdata.h dbgreceiver.h \
	dbgrequestor.h dbgrequestpack.h dbgrequestpackbuilder.h dbgresponsepack.h \
	dbgsettings.h dbgsettingswidget.h dbgstack.h debuggerdbg.h
