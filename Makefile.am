
INCLUDES=  -Ilibkhc -I$(srcdir)/libkhc $(all_includes) $(MICO_INCLUDES)
LDFLAGS = $(all_libraries) $(KDE_RPATH) $(MICO_LDFLAGS)
IDL_FLAGS = --relative-paths --any -I$(idldir)

bin_PROGRAMS = 	khelpcenter khcclient

khelpcenter_LDADD   = -lkparts $(LIB_KDEUI) $(LIB_KFILE) $(LIB_KHTML) libkhc/libkhc.la
khcclient_LDADD   = -lkparts -lkded libkhc/libkhc.la

service_DATA = khelpcenter.desktop
servicedir = $(kde_servicesdir)

EXTRA_DIST = $(service_DATA)

SUBDIRS = libkhc . kmanbrowser kinfobrowser kwelcome pics plugins

###### Files

khelpcenter_SOURCES = khelpcenter.idl khc_main.cc khc_application.cc \
	khc_mainwindow.cc khc_mainview.cc khc_htmlview.cc \
	khc_navigator.cc khc_navigatoritem.cc khc_searchwidget.cc \
	khc_indexwidget.cc khc_htmlsearch.cc khc_mansearch.cc

khelpcenter_METASOURCES = khc_mainwindow.moc khc_mainview.moc \
	khc_htmlview.moc khc_navigator.moc khc_indexwidget.moc \
	khc_searchwidget.moc

khcclient_SOURCES  = khelpcenter.cc khc_client.cc

noinst_HEADERS = khelpcenter.h khc_application.h khc_mainwindow.h \
	khc_mainview.h khc_htmlview.h khc_indexwidget.h \
	khc_mansearch.h khc_navigator.h khc_navigatoritem.h \
	khc_searchmatch.h khc_searchwidget.h version.h khc_htmlsearch.h

idl_DATA = khelpcenter.idl

$(srcdir)/khelpcenter.idl: $(idldir)/browser.idl

####### Build rules

messages:
	$(XGETTEXT) -C  -ktranslate -kaliasLocal -ki18n -x $(includedir)/kde.pot *.h *.cc *.cpp && mv messages.po ../po/khelpcenter.pot
	for subdir in $(SUBDIRS); do \
	   if test ! "x$$subdir" = "x."; then \
	   echo "Making messages in $$subdir"; \
	   (cd $$subdir && $(MAKE) -f Makefile.am messages) || exit 1; \
	 fi ;\
	done

