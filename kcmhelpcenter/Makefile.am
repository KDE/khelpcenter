
INCLUDES = $(all_includes) -I..
LDFLAGS  = $(all_libraries) $(KDE_RPATH)
LDADD    = $(LIB_KFM) $(LIB_KDEUI) 

#SUBDIRS = pics

bin_PROGRAMS              = kcmhelpcenter

kcmhelpcenter_SOURCES     = main.cpp assistant.cpp widget.cpp browser.cpp \
                            eyes.cpp

kcmhelpcenter_METASOURCES = USE_AUTOMOC

noinst_HEADERS            = assistant.h widget.h browser.h eyes.h

EXTRA_DIST                = kcmhelpcenter assistant.xpm mini-assistant.xpm


install-data-local: 
#	$(mkinstalldirs) $(kde_icondir)
#	$(INSTALL_DATA) assistant.xpm $(kde_icondir)
#	$(mkinstalldirs) $(kde_minidir)
#	$(INSTALL_DATA) mini-assistant.xpm $(kde_minidir)/assistant.xpm

uninstall-local:
#	rm -f $(kde_icondir)/assistant.xpm
#	rm -f $(kde_minidir)/assistant.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot *.cpp && mv messages.po ../../po/kcmhelpcenter.pot

DISTCLEANFILES = $(BUILT_SOURCES)
