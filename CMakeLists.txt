

add_subdirectory( plugins ) 
add_subdirectory( pics ) 
add_subdirectory( searchhandlers ) 
add_subdirectory( tests ) 


add_definitions (-DQT3_SUPPORT)

########### next target ###############

set(khc_indexbuilder_SRCS khc_indexbuilder.cpp )


kde4_add_executable(khc_indexbuilder ${khc_indexbuilder_SRCS})

target_link_libraries(khc_indexbuilder  ${KDE4_KDE3SUPPORT_LIBS} ${KDE4_KDECORE_LIBS} )

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

kde4_add_kdeinit_executable( khelpcenter ${khelpcenter_KDEINIT_SRCS})

target_link_libraries(kdeinit_khelpcenter  ${KDE4_KDE3SUPPORT_LIBS} ${KDE4_KHTML_LIBS}  ${QT_QTXML_LIBRARY}  )

install(TARGETS kdeinit_khelpcenter  DESTINATION ${LIB_INSTALL_DIR} )

target_link_libraries( khelpcenter kdeinit_khelpcenter )
install(TARGETS khelpcenter DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES Help.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES khelpcenter.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khelpcenter.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES khelpcenterui.rc intro.html.in glossary.html.in table-of-contents.xslt  	glossary.xslt index.html.in  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter )
install( FILES org.kde.khelpcenter.kcmhelpcenter.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
