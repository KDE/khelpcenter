INCLUDES= $(all_includes) $(MICO_INCLUDES)
LDFLAGS = $(all_libraries) $(KDE_RPATH) $(MICO_LDFLAGS)
IDL_FLAGS = --query-server-for-narrow --relative-paths --any -I$(idldir)

bin_PROGRAMS = 	khelpcenter khelpcenterclient

khelpcenter_LDADD   = -lkparts $(LIB_KDEUI) $(LIB_KFILE) $(LIB_KHTML)
khelpcenterclient_LDADD   = -lkparts -lkded

SUBDIRS = . pics plugins info2html rman kwelcome

###### Files

khelpcenter_SOURCES = browser.idl khelpcenter.idl khc_main.cc \
	khc_mainwindow.cc khc_baseview.cc khc_helpbrowser_impl.cc \
	khc_navigator.cc khc_indexwidget.cc khc_searchwidget.cc \
	khc_navigatoritem.cc khc_htmlsearch.cc khc_mansearch.cc \
	finddlg.cpp khc_htmlview.cc khc_history.cc

khelpcenter_METASOURCES = khc_mainwindow.moc \
	khc_navigator.moc khc_indexwidget.moc \
	khc_searchwidget.moc finddlg.moc khc_htmlview.moc \
	khc_testpart.moc

khelpcenterclient_SOURCES  = browser.cc khelpcenter.cc khc_client.cc

noinst_HEADERS = khelpcenter.h finddlg.h khc_baseview.h \
	khc_helpbrowser_impl.h khc_htmlsearch.h khc_htmlview.h khc_indexwidget.h \
	khc_mainwindow.h khc_mansearch.h khc_navigator.h khc_navigatoritem.h \
	khc_searchmatch.h khc_searchwidget.h man.h version.h khc_history.h

DISTCLEANFILES = $(khelpcenter_METASOURCES) khelpcenter.cc khelpcenter.h

idl_DATA = khelpcenter.idl

browser_IDLDIR = $(idldir)

####### Build rules

messages:
	$(XGETTEXT) -C  -ktranslate -kaliasLocal -ki18n -x $(includedir)/kde.pot *.h *.cc *.cpp && mv messages.po ../po/khelpcenter.pot
	for subdir in $(SUBDIRS); do \
	   if test ! "x$$subdir" = "x."; then \
	   echo "Making messages in $$subdir"; \
	   (cd $$subdir && $(MAKE) -f Makefile.am messages) || exit 1; \
	 fi ;\
	done
