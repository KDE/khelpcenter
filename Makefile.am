SUBDIRS = htmlsearch plugins pics 

INCLUDES = $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = 	khelpcenter

khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
khelpcenter_LDADD   = $(LIB_KDECORE)

khelpcenter_SOURCES = khc_main.cc KonquerorIface.stub

khc_main.cc: KonquerorIface_stub.h

lib_LTLIBRARIES = libkhelpcenter.la

libkhelpcenter_la_SOURCES = khc_factory.cc \
	khc_navigator.cc khc_navigatoritem.cc khc_searchwidget.cc 

libkhelpcenter_la_LIBADD = htmlsearch/libhtmlsearch.la -lkparts $(LIB_KDEUI)

libkhelpcenter_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined

service_DATA = khelpcenter.desktop
servicedir = $(kde_servicesdir)

data_DATA = Help.desktop
datadir = $(kde_appsdir)

EXTRA_DIST = $(service_DATA) $(data_DATA)

KonquerorIface_DIR = $(top_srcdir)/konqueror

messages:
	$(XGETTEXT) -kaliasLocal *.h *.cc -o $(podir)/khelpcenter.pot
