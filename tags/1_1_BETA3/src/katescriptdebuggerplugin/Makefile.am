INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/php -I$(top_srcdir)/src/php/dbg -I$(top_srcdir)/src/php/gubed \
	-I$(top_srcdir)/src/php/xdebug -I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/protoeditor/ui \
	-I$(top_srcdir)/src/settings -I$(top_srcdir)/src/settings/widgets $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = katescriptdebuggerplugin.la

katescriptdebuggerplugin_la_SOURCES = plugin_katescriptdebugger.cpp kateeditorui.cpp \
		kateuinterface.cpp katedebuggerui.cpp katedocwrapper.cpp

katescriptdebuggerplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

katescriptdebuggerplugin_la_LIBADD = $(top_builddir)/src/protoeditor/libprotoeditor.la \
	-lkateinterfaces $(LIB_KDEUI)

pluginsdir = $(kde_datadir)/kate/plugins/katescriptdebugger
plugins_DATA = ui.rc

kde_services_DATA = katescriptdebugger.desktop

noinst_HEADERS = kateeditorui.h kateuinterface.h katedebuggerui.h \
	katedocwrapper.h

# messages: rc.cpp
#        $(XGETTEXT) *.cpp -o $(podir)/katehelloworld.pot

# messages: rc.cpp
#	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/debugger.pot
