INCLUDES = $(all_includes) -I..
LDFLAGS  = $(all_libraries) $(KDE_RPATH)
LDADD    = $(LIB_KDEUI) 

bin_PROGRAMS = kcmhelpcenter

kcmhelpcenter_SOURCES = main.cpp assistant.cpp widget.cpp browser.cpp eyes.cpp

kcmhelpcenter_METASOURCES = AUTO

noinst_HEADERS = assistant.h widget.h browser.h eyes.h

EXTRA_DIST = kcmhelpcenter assistant.png mini-assistant.png

KDE_ICON = assistant

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kcmhelpcenter.pot

