project(KHelpCenter)
set(PROJECT_VERSION "5.5.4")

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.4.0")

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(ECMMarkNonGuiExecutable)
include(ECMOptionalAddSubdirectory)
include(FeatureSummary)
include(CheckIncludeFiles)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    DBus
    Widgets
    Xml
)

find_package(KF5 REQUIRED COMPONENTS
    Config
    CoreAddons
    DBusAddons
    I18n
    Init
    KCMUtils
    KDELibs4Support
    KHtml
    Service
)

add_subdirectory( plugins )
add_subdirectory( searchhandlers )
add_subdirectory( tests )
add_subdirectory( doc )

########### next target ###############

add_executable(khc_indexbuilder khc_indexbuilder.cpp)
target_compile_definitions(khc_indexbuilder PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
ecm_mark_nongui_executable(khc_indexbuilder)

target_link_libraries(khc_indexbuilder KF5::CoreAddons KF5::I18n Qt5::DBus)

install(TARGETS khc_indexbuilder DESTINATION ${LIBEXEC_INSTALL_DIR})

configure_file(config-khelpcenter.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-khelpcenter.h )

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

qt5_add_dbus_adaptor( khelpcenter_KDEINIT_SRCS org.kde.khelpcenter.kcmhelpcenter.xml kcmhelpcenter.h KCMHelpCenter )

kconfig_add_kcfg_files(khelpcenter_KDEINIT_SRCS prefs.kcfgc )
# kf5_add_app_icon(khelpcenter_KDEINIT_SRCS "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/apps/help-browser.png")

kf5_add_kdeinit_executable( khelpcenter ${khelpcenter_KDEINIT_SRCS})
target_compile_definitions(kdeinit_khelpcenter PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
target_link_libraries(kdeinit_khelpcenter KF5::KHtml KF5::KCMUtils KF5::Service KF5::DBusAddons KF5::KDELibs4Support Qt5::Xml)
install(TARGETS kdeinit_khelpcenter  ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(TARGETS khelpcenter ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( PROGRAMS org.kde.Help.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES khelpcenter.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khelpcenter.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES khelpcenter.desktop  DESTINATION  ${SHARE_INSTALL_PREFIX}/kde4/services )
install( FILES glossary.html.in table-of-contents.xslt glossary.xslt index.html.in DESTINATION ${DATA_INSTALL_DIR}/khelpcenter )
install( FILES khelpcenterui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/khelpcenter )
install( FILES org.kde.khelpcenter.kcmhelpcenter.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
