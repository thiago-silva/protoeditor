INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/commonphp -I$(top_srcdir)/src/dbg -I$(top_srcdir)/src/protoeditor -I../settings \
	$(all_includes)
METASOURCES = AUTO
libdbg_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libdbg.la
noinst_HEADERS = dbg_defs.h dbgconfiguration.h dbgconnection.h dbgframe.h \
  dbgheader.h dbgnethelper.h dbgreceiver.h dbgrequestor.h dbgrequestpack.h \
  dbgrequestpackbuilder.h dbgresponsepack.h dbgtags.h debuggerdbg.h dbgstack.h \
  dbgabstractdatarequest.h dbgfileinfo.h
libdbg_la_SOURCES = dbgconfiguration.cpp dbgconnection.cpp dbgframe.cpp \
  dbgheader.cpp dbgnethelper.cpp dbgreceiver.cpp dbgrequestor.cpp dbgrequestpack.cpp \
  dbgrequestpackbuilder.cpp dbgresponsepack.cpp dbgtags.cpp debuggerdbg.cpp dbgstack.cpp \
  dbgabstractdatarequest.cpp dbgfileinfo.cpp


libdbg_la_LIBADD = $(top_builddir)/src/settings/libsettings.la \
	$(top_builddir)/src/commonphp/libcommonphp.la $(top_builddir)/src/common/libcommon.la
