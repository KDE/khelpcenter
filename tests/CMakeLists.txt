
add_definitions (-DQT3_SUPPORT)

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

set(testmetainfo_SRCS
    testmetainfo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docmetainfo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docentry.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/../prefs.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docentrytraverser.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../htmlsearch.cpp )

kde4_automoc(${testmetainfo_SRCS})

kde4_add_test_executable(testmetainfo ${testmetainfo_SRCS})

target_link_libraries(testmetainfo  ${KDE4_KDEUI_LIBS} )

