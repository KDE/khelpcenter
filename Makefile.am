
INCLUDES= $(all_includes)

bin_PROGRAMS = 	khelpcenter

khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
khelpcenter_LDADD   = $(LIB_KDECORE)

#khcclient_LDADD   = -lkparts -lkded libkhc/libkhc.la

service_DATA = khelpcenter.desktop
servicedir = $(kde_servicesdir)

EXTRA_DIST = $(service_DATA)

#SUBDIRS = libkhc . kmanbrowser kinfobrowser kwelcome pics plugins
SUBDIRS = pics plugins

lib_LTLIBRARIES = libkhelpcenter.la

###### Files

khelpcenter_SOURCES = khc_main.cc KonquerorIface.stub

KonquerorIface_DIR =  $(top_srcdir)/konqueror

khc_main.cc: KonquerorIface_stub.h

libkhelpcenter_la_SOURCES = khc_factory.cc \
	khc_navigator.cc khc_navigatoritem.cc khc_searchwidget.cc \
	khc_indexwidget.cc khc_htmlsearch.cc khc_mansearch.cc kbrowser.moc.cc

METASOURCES = AUTO

libkhelpcenter_la_LIBADD = -lkparts $(LIB_KDEUI)

libkhelpcenter_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module

#khcclient_SOURCES  = khelpcenter.cc khc_client.cc

#noinst_HEADERS = khelpcenter.h khc_application.h khc_mainwindow.h \
#	khc_mainview.h khc_htmlview.h khc_indexwidget.h \
#	khc_mansearch.h khc_navigator.h khc_navigatoritem.h \
#	khc_searchmatch.h khc_searchwidget.h version.h khc_htmlsearch.h

kbrowser.moc.cc: $(kde_includes)/kbrowser.h
	$(MOC) $(kde_includes)/kbrowser.h -o $(top_builddir)/khelpcenter/kbrowser.moc.cc


####### Build rules

messages:
	$(XGETTEXT) -C  -ktranslate -kaliasLocal -ki18n -x $(includedir)/kde.pot *.h *.cc && mv messages.po $(podir)/khelpcenter.pot

check_PROGRAMS = linktest
linktest_SOURCES = khc_main.cc
linktest_LDADD = $(lib_LTLIBRARIES)

