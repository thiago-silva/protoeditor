INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
  -I$(top_srcdir)/src/commonphp -I$(top_srcdir)/src/dbg -I$(top_srcdir)/src/protoeditor \
  -I$(top_srcdir)/src/settings -I../settings $(all_includes)
METASOURCES = AUTO



libdbg_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libdbg.la

libdbg_la_SOURCES = dbgconnection.cpp dbgnet.cpp dbgnetdata.cpp dbgreceiver.cpp \
		dbgrequestor.cpp dbgrequestpack.cpp dbgrequestpackbuilder.cpp dbgresponsepack.cpp \
		dbgstack.cpp debuggerdbg.cpp dbgfileinfo.cpp dbgsettingswidget.cpp dbgsettings.cpp \
	dbgprofile.cpp

libdbg_la_LIBADD = $(top_builddir)/src/settings/libsettings.la \
  $(top_builddir)/src/commonphp/libcommonphp.la $(top_builddir)/src/common/libcommon.la
noinst_HEADERS = dbgfileinfo.h dbgsettingswidget.h dbgsettings.h dbgprofile.h
