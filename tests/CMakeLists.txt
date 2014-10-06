set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

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

########### next target ###############

kconfig_add_kcfg_files(khelpcenter_SRCS ../prefs.kcfgc )

set(testmetainfo_SRCS
    testmetainfo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docmetainfo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docentry.cpp 
    ${khelpcenter_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../docentrytraverser.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../htmlsearch.cpp )


add_executable(testmetainfo ${testmetainfo_SRCS})
ecm_mark_as_test(testmetainfo)

target_link_libraries(testmetainfo  KF5::KDELibs4Support )

