SUBDIRS = htmlsearch plugins pics 

INCLUDES = $(all_includes)
METASOURCES = AUTO

# khelpcenter: binary for convenience
bin_PROGRAMS = 	khelpcenter

khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
khelpcenter_LDADD   = $(LIB_KHTML)
khelpcenter_SOURCES = main.cpp khc_main.cc $(libkhelpcenterpart_la_SOURCES)

kde_module_LTLIBRARIES = libkhelpcenterpart.la
libkhelpcenterpart_la_SOURCES = khc_factory.cc khc_navigator.cc \
	khc_navigatoritem.cc 
libkhelpcenterpart_la_LIBADD = htmlsearch/libhtmlsearch.la $(LIB_KPARTS)
libkhelpcenterpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)

service_DATA = khelpcenter.desktop
servicedir = $(kde_servicesdir)

data_DATA = Help.desktop
datadir = $(kde_appsdir)

rc_DATA = khelpcenterui.rc
rcdir = $(kde_datadir)/khelpcenter

messages:
	$(XGETTEXT) -C $(libkhelpcenterpart_la_SOURCES) -o $(podir)/khelpcenter.pot

