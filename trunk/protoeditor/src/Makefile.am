# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src -I$(top_srcdir)/src/common \
  -I$(top_srcdir)/src/commonphp -I$(top_srcdir)/src/dbg $(all_includes)

# these are the headers for your project
noinst_HEADERS = editortabwidget.h debuggerconfigurations.h debuggerfactory.h \
	debuggermanager.h variableslistview.h debuggercombostack.h variableslistviewitem.h \
	loglistview.h watchlistview.h breakpointlistview.h breakpointlistviewitem.h \
	mainwindow.h debuggersettingswidget.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

##messages: rc.cpp
## $(XGETTEXT) *.cpp -o $(podir)/protoeditor.pot


# this is where the desktop file will go
shelldesktopdir   = $(kde_appsdir)/Development
shelldesktop_DATA = protoeditor.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/protoeditor
shellrc_DATA = protoeditorui.rc

# Application settings data
kde_kcfg_DATA = protoeditor.kcfg



bin_PROGRAMS = protoeditor
protoeditor_LDFLAGS = $(KDE_RPATH) $(all_libraries)
protoeditor_SOURCES = editortabwidget.cpp debuggerconfigurations.cpp \
	debuggerfactory.cpp debuggermanager.cpp variableslistview.cpp debuggercombostack.cpp \
	variableslistviewitem.cpp loglistview.cpp watchlistview.cpp breakpointlistview.cpp \
	breakpointlistviewitem.cpp mainwindow.cpp protoeditor.cpp debuggersettingswidget.cpp \
	debuggersettings.kcfgc

SUBDIRS = commonphp common dbg
protoeditor_LDADD = $(top_builddir)/src/common/libcommon.la \
  $(top_builddir)/src/commonphp/libcommonphp.la $(top_builddir)/src/dbg/libdbg.la -lkatepartinterfaces $(LIB_KDEUI)
