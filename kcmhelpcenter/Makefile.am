
INCLUDES = $(all_includes) -I..
LDFLAGS  = $(all_libraries) $(KDE_RPATH)
LDADD    = $(LIB_KDEUI) 

#SUBDIRS = pics

bin_PROGRAMS              = kcmhelpcenter

kcmhelpcenter_SOURCES     = main.cpp assistant.cpp widget.cpp browser.cpp \
                            eyes.cpp

kcmhelpcenter_METASOURCES = AUTO

noinst_HEADERS            = assistant.h widget.h browser.h eyes.h

EXTRA_DIST                = kcmhelpcenter assistant.png mini-assistant.png

KDE_ICON = assistant

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot *.cpp && mv messages.po $(podir)/kcmhelpcenter.pot

