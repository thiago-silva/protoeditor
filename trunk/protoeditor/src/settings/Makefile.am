INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/settings $(all_includes)
METASOURCES = AUTO

# Application settings data
kde_kcfg_DATA = debugger.kcfg browser.kcfg


noinst_LTLIBRARIES = libsettings.la
libsettings_la_LDFLAGS = $(all_libraries)
libsettings_la_SOURCES = debuggersettings.kcfgc debuggersettingswidget.cpp \
    browsersettingswidget.cpp browsersettings.kcfgc browser.kcfg
noinst_HEADERS = debuggersettingswidget.h browsersettingswidget.h
