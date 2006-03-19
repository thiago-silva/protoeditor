INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common   -I$(top_srcdir)/src/commonphp -I$(top_srcdir)/src/dbg -I$(top_srcdir)/src/protoeditor   -I$(top_srcdir)/src/settings -I../settings $(all_includes)
METASOURCES = AUTO
libdbgprofiler_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
noinst_LTLIBRARIES = libdbgprofiler.la
noinst_HEADERS = dbgprofilelocation.h dbgprofiledata.h dbgprofiledialog.h \
	dbgprofilelistview.h
libdbgprofiler_la_SOURCES = dbgprofilelocation.cpp dbgprofiledata.cpp \
	dbgprofiledialog.cpp dbgprofilelistview.cpp
