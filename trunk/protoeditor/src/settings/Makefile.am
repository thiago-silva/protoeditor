INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/settings $(all_includes)
METASOURCES = AUTO

# Application settings data
kde_kcfg_DATA = settings.kcfg


noinst_LTLIBRARIES = libsettings.la
libsettings_la_LDFLAGS = $(all_libraries)
libsettings_la_SOURCES = settings.kcfgc debuggersettingswidget.cpp \
    browsersettingswidget.cpp
noinst_HEADERS = debuggersettingswidget.h browsersettingswidget.h
