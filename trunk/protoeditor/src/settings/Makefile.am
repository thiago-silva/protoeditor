INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/settings $(all_includes)
METASOURCES = AUTO

# Application settings data


noinst_LTLIBRARIES = libsettings.la

libsettings_la_LDFLAGS = $(all_libraries)

SUBDIRS = widgets
libsettings_la_SOURCES = protoeditorsettings.cpp extappsettings.cpp sitesettings.cpp languagesettings.cpp
noinst_HEADERS = debuggersettingsinterface.h protoeditorsettings.h extappsettings.h sitesettings.h languagesettings.h

