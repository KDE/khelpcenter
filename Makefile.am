
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
libkhelpcenter_common_la_SOURCES = navigator.cpp \
	navigatoritem.cpp navigatorappitem.cpp view.cpp \
	infohierarchymaker.cpp inforeader.cpp infofile.cpp \
	infonode.cpp searchwidget.cpp klanguagebutton.cpp searchengine.cpp \
	docmetainfo.cpp docentrytraverser.cpp searchformatter.cpp \
	glossary.cpp toc.cpp mainwindow.cpp docentry.cpp htmlsearch.cpp

kcm_helpcenter_la_LDFLAGS = $(KDE_RPATH) -module -avoid-version \
                            $(all_libraries) -no-undefined
kcm_helpcenter_la_LIBADD  = $(LIB_KDEUI)
kcm_helpcenter_la_SOURCES = kcmhelpcenter.cpp docmetainfo.cpp \
                            docentrytraverser.cpp docentry.cpp htmlsearch.cpp \
                            htmlsearchconfig.cpp

check_PROGRAMS = testmetainfo

testmetainfo_LDFLAGS = $(all_libraries) $(KDE_RPATH)
testmetainfo_LDADD = $(LIB_KDECORE) docentry.lo docmetainfo.lo \
                     docentrytraverser.lo htmlsearch.lo
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
