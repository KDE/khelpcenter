SUBDIRS = plugins pics searchhandlers

INCLUDES = $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = khc_indexbuilder
lib_LTLIBRARIES = 

khc_indexbuilder_LDFLAGS = $(all_libraries) $(KDE_RPATH)
khc_indexbuilder_LDADD = $(LIB_KDECORE)
khc_indexbuilder_SOURCES = khc_indexbuilder.cpp

kdeinit_LTLIBRARIES = khelpcenter.la

khelpcenter_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
khelpcenter_la_LIBADD = $(LIB_KHTML)
khelpcenter_la_SOURCES = navigator.cpp \
	navigatoritem.cpp navigatorappitem.cpp view.cpp \
	searchwidget.cpp searchengine.cpp \
	docmetainfo.cpp docentrytraverser.cpp formatter.cpp \
	glossary.cpp toc.cpp mainwindow.cpp docentry.cpp htmlsearch.cpp \
	history.cpp application.cpp searchwidget.skel treebuilder.cpp \
	infotree.cpp  mainwindow.skel \
        kcmhelpcenter.cpp htmlsearchconfig.cpp kcmhelpcenter.skel \
        fontdialog.cpp plugintraverser.cpp scrollkeepertreebuilder.cpp \
        prefs.kcfgc searchhandler.cpp

#kcm_helpcenter_la_LDFLAGS = $(KDE_RPATH) -module -avoid-version \
#                            $(all_libraries) -no-undefined
#kcm_helpcenter_la_LIBADD  = $(LIB_KIO)
#kcm_helpcenter_la_SOURCES = kcmhelpcenter.cpp docmetainfo.cpp \
#                            docentrytraverser.cpp docentry.cpp htmlsearch.cpp \
#                            htmlsearchconfig.cpp

EXTRA_PROGRAMS = testmetainfo

testmetainfo_LDFLAGS = $(all_libraries) $(KDE_RPATH)
testmetainfo_LDADD = $(LIB_KDECORE) docentry.lo docmetainfo.lo \
                     docentrytraverser.lo htmlsearch.lo
testmetainfo_SOURCES = testmetainfo.cpp

service_DATA = khelpcenter.desktop
servicedir = $(kde_servicesdir)

xdg_apps_DATA = Help.desktop

#xdg_apps_DATA = kcmhelpcenter.desktop

rc_DATA = khelpcenterui.rc intro.html.in glossary.html.in table-of-contents.xslt \
	glossary.xslt
rcdir = $(kde_datadir)/khelpcenter

kde_kcfg_DATA = khelpcenter.kcfg

messages: rc.cpp
	$(EXTRACTRC) `find . -name "*.kcfg"` >> rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/khelpcenter.pot
