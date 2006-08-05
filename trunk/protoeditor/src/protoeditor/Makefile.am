SUBDIRS = ui

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/perl -I$(top_srcdir)/src/perl/perldb -I$(top_srcdir)/src/perl/perldbgp \
	-I$(top_srcdir)/src/php -I$(top_srcdir)/src/php/dbg -I$(top_srcdir)/src/php/gubed \
	-I$(top_srcdir)/src/php/xdebug -I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/protoeditor/ui \
	-I$(top_srcdir)/src/python -I$(top_srcdir)/src/settings -I$(top_srcdir)/src/settings/widgets \
	-I../settings $(all_includes)

# these are the headers for your project
noinst_HEADERS = debuggerfactory.h protoeditor.h 		executioncontroller.h \
			datacontroller.h uinterface.h editorinterface.h debuggerinterface.h


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

noinst_LTLIBRARIES = libprotoeditor.la
libprotoeditor_la_SOURCES = debuggerfactory.cpp protoeditor.cpp \
			executioncontroller.cpp datacontroller.cpp uinterface.cpp editorinterface.cpp \
	debuggerinterface.cpp
libprotoeditor_la_LDFLAGS = $(all_libraries) \
	$(top_builddir)/src/protoeditor/ui/libui.la \
	$(top_builddir)/src/settings/widgets/libsettingswidget.la \
	$(top_builddir)/src/settings/libsettings.la \
	$(top_builddir)/src/common/libcommon.la \
	$(top_builddir)/src/common/php/libcommonphp.la \
	$(top_builddir)/src/php/libphplang.la \
	$(top_builddir)/src/php/dbg/profiler/libdbgprofiler.la \
	$(top_builddir)/src/php/gubed/libgubed.la \
	$(top_builddir)/src/php/xdebug/libxdebug.la \
	$(top_builddir)/src/php/dbg/libdbg.la \
	$(top_builddir)/src/common/perl/libcommonperl.la \
	$(top_builddir)/src/perl/perldbgp/libperldbgp.la \
	$(top_builddir)/src/perl/libperllang.la \
	$(top_builddir)/src/common/python/libcommonpython.la \
	$(top_builddir)/src/python/libpythonlang.la \
	$(top_builddir)/src/python/pythondbgp/libpythondbgp.la

if !KATE_PLUGIN
shelldesktopdir = $(kde_appsdir)/Development
shelldesktop_DATA = protoeditor.desktop

shellrcdir = $(kde_datadir)/protoeditor
shellrc_DATA = protoeditorui.rc

bin_PROGRAMS = protoeditor
protoeditor_LDFLAGS = $(KDE_RPATH) $(all_libraries)
protoeditor_SOURCES = main.cpp
protoeditor_LDADD = $(top_builddir)/src/protoeditor/libprotoeditor.la \
	-lkatepartinterfaces $(LIB_KDEUI)
endif