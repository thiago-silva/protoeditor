INCLUDES = -I$(top_srcdir)/src/nodeclasses -I$(top_srcdir)/src/parser -Iparser\
	$(all_includes)
METASOURCES = AUTO
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = katephpbrowserplugin.la

katephpbrowserplugin_la_SOURCES = plugin_katephpbrowser.cpp browsertab.cpp \
		browserlistview.cpp schemaconfigurationdlg.cpp schemasettings.cpp schema.cpp
katephpbrowserplugin_la_LIBADD = $(top_builddir)/src/parser/libphpparser.la \
	$(top_builddir)/src/nodeclasses/libnodes.la -lkateinterfaces
katephpbrowserplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kate/plugins/katephpbrowser
# plugins_DATA = plugin_katephpbrowser.rc

kde_services_DATA = katephpbrowser.desktop

# messages: rc.cpp
# 	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
# 	$(XGETTEXT) *.cpp -o $(podir)/katephpbrowser.pot
noinst_HEADERS = browsertab.h browserlistview.h schemaconfigurationdlg.h \
	schemasettings.h schema.h
SUBDIRS = parser nodeclasses
