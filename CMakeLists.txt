
project(khelpcenter)

add_subdirectory( plugins ) 
add_subdirectory( pics ) 
add_subdirectory( searchhandlers ) 



########### next target ###############

set(khc_indexbuilder_SRCS khc_indexbuilder.cpp )

kde4_automoc(${khc_indexbuilder_SRCS})

kde4_add_executable(khc_indexbuilder ${khc_indexbuilder_SRCS})

target_link_libraries(khc_indexbuilder  ${KDE4_KDECORE_LIBS} )

install(TARGETS khc_indexbuilder DESTINATION ${BIN_INSTALL_DIR})


########### next target ###############

set(khelpcenter_KDEINIT_SRCS 
   navigator.cpp 
   navigatoritem.cpp 
   navigatorappitem.cpp 
   view.cpp 
   searchwidget.cpp 
   searchengine.cpp 
   docmetainfo.cpp 
   docentrytraverser.cpp 
   formatter.cpp 
   glossary.cpp 
   toc.cpp 
   mainwindow.cpp 
   docentry.cpp 
   htmlsearch.cpp 
   history.cpp 
   application.cpp 
   treebuilder.cpp 
   infotree.cpp 
   kcmhelpcenter.cpp 
   htmlsearchconfig.cpp 
   fontdialog.cpp 
   plugintraverser.cpp 
   scrollkeepertreebuilder.cpp 
   searchhandler.cpp )

qt4_add_dbus_adaptor( khelpcenter_KDEINIT_SRCS org.kde.khelpcenter.kcmhelpcenter.xml kcmhelpcenter.h KCMHelpCenter )
kde4_automoc(${khelpcenter_KDEINIT_SRCS})


kde4_add_kcfg_files(khelpcenter_KDEINIT_SRCS prefs.kcfgc )

kde4_add_kdeinit_executable( khelpcenter ${khelpcenter_KDEINIT_SRCS})

target_link_libraries(kdeinit_khelpcenter  ${KDE4_KDE3SUPPORT_LIBS} khtml  ${QT_QTXML_LIBRARY}  )

install(TARGETS kdeinit_khelpcenter  DESTINATION ${LIB_INSTALL_DIR} )

target_link_libraries( khelpcenter kdeinit_khelpcenter )
install(TARGETS khelpcenter DESTINATION ${BIN_INSTALL_DIR})

########### next target ###############

if(KDE4_BUILD_TESTS)
set(testmetainfo_SRCS testmetainfo.cpp docmetainfo.cpp docentry.cpp prefs.cpp docentrytraverser.cpp htmlsearch.cpp )

kde4_automoc(${testmetainfo_SRCS})

kde4_add_executable(testmetainfo ${testmetainfo_SRCS})

target_link_libraries(testmetainfo  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### install files ###############

install( FILES Help.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES khelpcenter.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khelpcenter.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES khelpcenterui.rc intro.html.in glossary.html.in table-of-contents.xslt  	glossary.xslt index.html.in  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter )




#original Makefile.am contents follow:

#SUBDIRS = plugins pics searchhandlers
#
#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#
#bin_PROGRAMS = khc_indexbuilder
#lib_LTLIBRARIES = 
#
#khc_indexbuilder_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#khc_indexbuilder_LDADD = $(LIB_KDECORE)
#khc_indexbuilder_SOURCES = khc_indexbuilder.cpp
#
#kdeinit_LTLIBRARIES = khelpcenter.la
#
#khelpcenter_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#khelpcenter_la_LIBADD = $(LIB_KHTML)
#khelpcenter_la_SOURCES = navigator.cpp \
#	navigatoritem.cpp navigatorappitem.cpp view.cpp \
#	searchwidget.cpp searchengine.cpp \
#	docmetainfo.cpp docentrytraverser.cpp formatter.cpp \
#	glossary.cpp toc.cpp mainwindow.cpp docentry.cpp htmlsearch.cpp \
#	history.cpp application.cpp searchwidget.skel treebuilder.cpp \
#	infotree.cpp  mainwindow.skel \
#        kcmhelpcenter.cpp htmlsearchconfig.cpp kcmhelpcenter.skel \
#        fontdialog.cpp plugintraverser.cpp scrollkeepertreebuilder.cpp \
#        prefs.kcfgc searchhandler.cpp
#
##kcm_helpcenter_la_LDFLAGS = $(KDE_RPATH) -module -avoid-version \
##                            $(all_libraries) -no-undefined
##kcm_helpcenter_la_LIBADD  = $(LIB_KIO)
##kcm_helpcenter_la_SOURCES = kcmhelpcenter.cpp docmetainfo.cpp \
##                            docentrytraverser.cpp docentry.cpp htmlsearch.cpp \
##                            htmlsearchconfig.cpp
#
#EXTRA_PROGRAMS = testmetainfo
#
#testmetainfo_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#testmetainfo_LDADD = $(LIB_KDECORE) docentry.lo docmetainfo.lo \
#                     docentrytraverser.lo htmlsearch.lo
#testmetainfo_SOURCES = testmetainfo.cpp
#
#service_DATA = khelpcenter.desktop
#servicedir = $(kde_servicesdir)
#
#xdg_apps_DATA = Help.desktop
#
##xdg_apps_DATA = kcmhelpcenter.desktop
#
#rc_DATA = khelpcenterui.rc intro.html.in glossary.html.in table-of-contents.xslt \
#	glossary.xslt index.html.in
#rcdir = $(kde_datadir)/khelpcenter
#
#kde_kcfg_DATA = khelpcenter.kcfg
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name "*.kcfg"` >> rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/khelpcenter.pot
