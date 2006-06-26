# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/common -I$(top_srcdir)/src/php/dbg \
	-I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/settings -I$(top_srcdir)/src/settings/widgets \
	-I$(top_srcdir)/src/php/xdebug -I$(top_srcdir)/src/php/gubed -I../settings \
	-I$(top_srcdir)/src/perl/perldebugger $(all_includes)

# these are the headers for your project
noinst_HEADERS = editortabwidget.h debuggerfactory.h debuggermanager.h \
			variableslistview.h debuggercombostack.h variableslistviewitem.h messagelistview.h \
			watchlistview.h breakpointlistview.h breakpointlistviewitem.h mainwindow.h document.h \
	watchwidget.h


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

##messages: rc.cpp
## $(XGETTEXT) *.cpp -o $(podir)/protoeditor.pot


# this is where the desktop file will go
shelldesktopdir = $(kde_appsdir)/Development
shelldesktop_DATA = protoeditor.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/protoeditor
shellrc_DATA = protoeditorui.rc

# Application settings data
#kde_kcfg_DATA = protoeditor.kcfg



bin_PROGRAMS = protoeditor
protoeditor_LDFLAGS = $(KDE_RPATH) $(all_libraries)
protoeditor_SOURCES = editortabwidget.cpp debuggerfactory.cpp \
			debuggermanager.cpp variableslistview.cpp debuggercombostack.cpp variableslistviewitem.cpp \
			messagelistview.cpp watchlistview.cpp breakpointlistview.cpp breakpointlistviewitem.cpp \
			mainwindow.cpp protoeditor.cpp document.cpp watchwidget.cpp

protoeditor_LDADD = $(top_builddir)/src/php/gubed/libgubed.la \
	$(top_builddir)/src/php/xdebug/libxdebug.la $(top_builddir)/src/settings/widgets/libsettingswidget.la \
	$(top_builddir)/src/settings/libsettings.la $(top_builddir)/src/php/dbg/profiler/libdbgprofiler.la \
	$(top_builddir)/src/common/php/libcommonphp.la $(top_builddir)/src/common/libcommon.la \
	$(top_builddir)/src/php/dbg/libdbg.la $(top_builddir)/src/perl/perldebugger/libperldebugger.la \
	-lkatepartinterfaces $(LIB_KDEUI)
