add_subdirectory( plugins )
add_subdirectory( searchhandlers )
add_subdirectory( tests )

########### next target ###############

set(khc_indexbuilder_SRCS khc_indexbuilder.cpp )


kde4_add_executable(khc_indexbuilder NOGUI ${khc_indexbuilder_SRCS})

target_link_libraries(khc_indexbuilder ${KDE4_KDECORE_LIBS} )

install(TARGETS khc_indexbuilder DESTINATION ${LIBEXEC_INSTALL_DIR})


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

kde4_add_kcfg_files(khelpcenter_KDEINIT_SRCS prefs.kcfgc )
kde4_add_app_icon(khelpcenter_KDEINIT_SRCS "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/apps/help-browser.png")

kde4_add_kdeinit_executable( khelpcenter ${khelpcenter_KDEINIT_SRCS})
target_link_libraries(kdeinit_khelpcenter  ${KDE4_KHTML_LIBS}  ${QT_QTXML_LIBRARY} ${KDE4_KCMUTILS_LIBS} )
install(TARGETS kdeinit_khelpcenter  ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(TARGETS khelpcenter ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( PROGRAMS Help.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES khelpcenter.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khelpcenter.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES khelpcenterui.rc glossary.html.in table-of-contents.xslt  	glossary.xslt index.html.in  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter )
install( FILES org.kde.khelpcenter.kcmhelpcenter.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
