
SUBDIRS = plugins pics

INCLUDES = $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = khelpcenter

khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
khelpcenter_LDADD   = libkhelpcenter_common.la
khelpcenter_SOURCES = main.cpp

noinst_LTLIBRARIES = libkhelpcenter_common.la
kde_module_LTLIBRARIES = khelpcenter.la kcm_helpcenter.la

khelpcenter_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module $(KDE_PLUGIN) -no-undefined
khelpcenter_la_LIBADD  = libkhelpcenter_common.la
khelpcenter_la_SOURCES = dummy.cpp

dummy.cpp:
	echo > $@

libkhelpcenter_common_la_LDFLAGS = $(all_libraries) -no-undefined
libkhelpcenter_common_la_LIBADD = $(LIB_KHTML)
libkhelpcenter_common_la_SOURCES = khc_factory.cc khc_navigator.cc \
	khc_navigatoritem.cc khc_navigatorappitem.cc khc_view.cc \
	khc_infohierarchymaker.cc khc_inforeader.cc khc_infofile.cc \
	khc_infonode.cc searchwidget.cpp klanguagebutton.cpp searchengine.cpp \
	docmetainfo.cpp docentry.cpp docentrytraverser.cpp searchformatter.cpp \
	khc_glossary.cc khc_toc.cc khc_main.cc

kcm_helpcenter_la_LDFLAGS = $(KDE_RPATH) -module -avoid-version \
                            $(all_libraries) -no-undefined
kcm_helpcenter_la_LIBADD  = $(LIB_KDEUI)
kcm_helpcenter_la_SOURCES = kcmhelpcenter.cpp docmetainfo.cpp docentry.cpp \
                            docentrytraverser.cpp

check_PROGRAMS = testmetainfo

testmetainfo_LDFLAGS = $(all_libraries) $(KDE_RPATH)
testmetainfo_LDADD = $(LIB_KDECORE) docentry.lo docmetainfo.lo docentrytraverser.lo
testmetainfo_SOURCES = testmetainfo.cpp
data_DATA = Help.desktop
datadir = $(kde_appsdir)

kcmhelpcenter_DATA = kcmhelpcenter.desktop
kcmhelpcenterdir = $(kde_appsdir)/Settings/Help

rc_DATA = khelpcenterui.rc intro.html.in glossary.html.in table-of-contents.xslt \
	glossary.xslt
rcdir = $(kde_datadir)/khelpcenter

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/khelpcenter.pot
