# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/commonphp -I$(top_srcdir)/src/dbg $(all_includes)

# these are the headers for your project
noinst_HEADERS = protoeditor.h editortabwidget.h mainwindow.h mainwindowbase.h \
	dbugrconfigform.h debuggerconfigurations.h debuggerfactory.h debuggermanager.h \
	variableslistview.h debuggercombostack.h variableslistviewitem.h loglistview.h \
	watchlistview.h breakpointlistview.h breakpointlistviewitem.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/protoeditor.pot


#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# the application source, library search path, and link libraries

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities


# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/protoeditor



bin_PROGRAMS = protoeditor
protoeditor_LDFLAGS = $(KDE_RPATH) $(all_libraries)
protoeditor_SOURCES = dbugrconfigform.cpp dbugrconfigformbase.ui \
		editortabwidget.cpp main.cpp mainwindow.cpp mainwindowbase.cpp protoeditor.cpp \
		debuggerconfigurations.cpp debuggerfactory.cpp debuggermanager.cpp variableslistview.cpp \
		debuggercombostack.cpp variableslistviewitem.cpp loglistview.cpp watchlistview.cpp \
	breakpointlistview.cpp breakpointlistviewitem.cpp

SUBDIRS = commonphp common dbg
protoeditor_LDADD = $(top_builddir)/src/common/libcommon.la \
	$(top_builddir)/src/commonphp/libcommonphp.la $(top_builddir)/src/dbg/libdbg.la -lkatepartinterfaces $(LIB_KDEUI)
