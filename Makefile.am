SUBDIRS = plugins pics

INCLUDES = $(all_includes)
METASOURCES = AUTO


bin_PROGRAMS = khelpcenter

khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
khelpcenter_LDADD   = $(LIB_KHTML)
khelpcenter_SOURCES = main.cpp khc_main.cc $(libkhelpcenterpart_la_SOURCES)


kde_module_LTLIBRARIES = libkhelpcenterpart.la khelpcenter.la kcm_helpcenter.la

khelpcenter_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module $(KDE_PLUGIN) -no-undefined
khelpcenter_la_LIBADD  = $(LIB_KHTML)
khelpcenter_la_SOURCES = khc_main.cc $(libkhelpcenterpart_la_SOURCES)

libkhelpcenterpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkhelpcenterpart_la_LIBADD = $(LIB_KHTML) $(LIB_KPARTS)
libkhelpcenterpart_la_SOURCES = khc_factory.cc khc_navigator.cc \
	khc_navigatoritem.cc khc_navigatorappitem.cc khc_view.cc \
	khc_infohierarchymaker.cc khc_inforeader.cc khc_infofile.cc \
        khc_infonode.cc searchwidget.cpp klanguagebutton.cpp searchengine.cpp \
        docmetainfo.cpp

kcm_helpcenter_la_LDFLAGS = $(KDE_RPATH) -module -avoid-version \
                            $(all_libraries) -no-undefined
kcm_helpcenter_la_LIBADD  = $(LIB_KDEUI)
kcm_helpcenter_la_SOURCES = kcmhelpcenter.cpp docmetainfo.cpp


check_PROGRAMS = testmetainfo

testmetainfo_LDFLAGS = $(all_libraries) $(KDE_RPATH)
testmetainfo_LDADD = kcm_helpcenter.la
testmetainfo_SOURCES = testmetainfo.cpp

service_DATA = khelpcenter.desktop
servicedir = $(kde_servicesdir)

data_DATA = Help.desktop
datadir = $(kde_appsdir)

kcmhelpcenter_DATA = kcmhelpcenter.desktop
kcmhelpcenterdir = $(kde_appsdir)/Settings/Help

rc_DATA = khelpcenterui.rc intro.html.in glossary.html.in
rcdir = $(kde_datadir)/khelpcenter

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/khelpcenter.pot
