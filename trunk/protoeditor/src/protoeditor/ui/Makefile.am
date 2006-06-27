INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/src/protoeditor -I$(top_srcdir)/src/settings -I$(top_srcdir)/src/settings/widgets \
	-I../settings $(all_includes) -I$(top_srcdir)/src/protoeditor/ui -I$(top_srcdir)/src/common

METASOURCES = AUTO
libui_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libui.la

libui_la_SOURCES = debuggerui.cpp breakpointlistview.cpp \
		breakpointlistviewitem.cpp combostack.cpp document.cpp editorui.cpp localtab.cpp 	mainwindow.cpp \
	messagelistview.cpp variablelistview.cpp variablelistviewitem.cpp 	watchlistview.cpp \
	watchtab.cpp statusbarwidget.cpp
noinst_HEADERS = breakpointlistview.h breakpointlistviewitem.h \
	combostack.h	debuggerui.h document.h editorui.h localtab.h mainwindow.h	messagelistview.h \
	variablelistview.h variablelistviewitem.h	watchlistview.h watchtab.h statusbarwidget.h
