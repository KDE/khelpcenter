SUBDIRS = htmlsearch plugins pics 

INCLUDES = $(all_includes)
METASOURCES = AUTO

# khelpcenter: binary for convenience
bin_PROGRAMS = 	khelpcenter

khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
khelpcenter_LDADD   = khelpcenter.la

khelpcenter_SOURCES = dummy.cc

# libkhelpcenterpart.la: The helpcenter KPart
# khelpcenter.la: the helpcenter as kdeinit loadable module
lib_LTLIBRARIES = libkhelpcenterpart.la khelpcenter.la

khelpcenter_la_LDFLAGS = $(all_libraries) -module -avoid-version
khelpcenter_la_LIBADD   = $(LIB_KDECORE)

khelpcenter_la_SOURCES = khc_main.cc KonquerorIface.stub

libkhelpcenterpart_la_SOURCES = khc_factory.cc \
	khc_navigator.cc khc_navigatoritem.cc khc_searchwidget.cc \
	klangcombo.cpp ktagcombobox.cpp

libkhelpcenterpart_la_LIBADD = htmlsearch/libhtmlsearch.la -lkparts $(LIB_KDEUI)

libkhelpcenterpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)

service_DATA = khelpcenter.desktop
servicedir = $(kde_servicesdir)

data_DATA = Help.desktop
datadir = $(kde_appsdir)

EXTRA_DIST = $(service_DATA) $(data_DATA)

KonquerorIface_DIR = $(top_srcdir)/konqueror

messages:
	$(XGETTEXT) -C $(libkhelpcenterpart_la_SOURCES) -o $(podir)/khelpcenter.pot

khc_main.cc: KonquerorIface_stub.h

dummy.cc:
	echo > dummy.cc
