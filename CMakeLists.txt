cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(KHelpCenter)
set(PROJECT_VERSION "5.7.3")

set(QT_MIN_VERSION "5.9.0")

set(KF5_MIN_VERSION "5.40.0")
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(ECMMarkNonGuiExecutable)
include(ECMOptionalAddSubdirectory)
include(FeatureSummary)
include(CheckIncludeFiles)
include(ECMQtDeclareLoggingCategory)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    DBus
    Widgets
    Xml
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    Bookmarks
    Config
    CoreAddons
    DBusAddons
    DocTools
    I18n
    Init
    KHtml
    Service
    WindowSystem
)

find_package(Grantlee5 REQUIRED)
set_package_properties(Grantlee5 PROPERTIES
    DESCRIPTION "Support for HTML templates"
    URL "http://grantlee.org/"
    TYPE REQUIRED)

find_package(Xapian REQUIRED)
set_package_properties(Xapian PROPERTIES
    DESCRIPTION "Support for text indexing and searching"
    URL "https://xapian.org/"
    TYPE REQUIRED)

find_package(LibXml2 REQUIRED)
set_package_properties(LibXml2 PROPERTIES
    DESCRIPTION "Support for extracting text from HTML documents"
    URL "http://www.xmlsoft.org/"
    TYPE REQUIRED)

add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
#add_definitions(
#  -DQT_USE_QSTRINGBUILDER
#  -DQT_NO_CAST_TO_ASCII
#  -DQT_NO_CAST_FROM_ASCII
#  -DQT_NO_URL_CAST_FROM_STRING
#  -DQT_NO_CAST_FROM_BYTEARRAY
#  -DQT_NO_SIGNALS_SLOTS_KEYWORDS
#)

add_subdirectory( plugins )
add_subdirectory( searchhandlers )
add_subdirectory( tests )
add_subdirectory( doc )
add_subdirectory( templates )

########### next target ###############

set(khelpcenter_KDEINIT_SRCS
   navigator.cpp
   navigatoritem.cpp
   navigatorappitem.cpp
   navigatorappgroupitem.cpp
   view.cpp
   searchwidget.cpp
   searchengine.cpp
   docmetainfo.cpp
   docentrytraverser.cpp
   grantleeformatter.cpp
   glossary.cpp
   toc.cpp
   mainwindow.cpp
   docentry.cpp
   history.cpp
   application.cpp
   treebuilder.cpp
   infotree.cpp
   fontdialog.cpp
   plugintraverser.cpp
   scrollkeepertreebuilder.cpp
   bookmarkowner.cpp
   searchhandler.cpp )
ecm_qt_declare_logging_category(khelpcenter_KDEINIT_SRCS HEADER khc_debug.h IDENTIFIER KHC_LOG CATEGORY_NAME org.kde.khelpcenter)

kconfig_add_kcfg_files(khelpcenter_KDEINIT_SRCS prefs.kcfgc )
# kf5_add_app_icon(khelpcenter_KDEINIT_SRCS "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/apps/help-browser.png")

kf5_add_kdeinit_executable( khelpcenter ${khelpcenter_KDEINIT_SRCS})
target_compile_definitions(kdeinit_khelpcenter PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
target_link_libraries(kdeinit_khelpcenter KF5::KHtml KF5::Service KF5::DBusAddons KF5::ConfigGui KF5::WindowSystem KF5::Bookmarks Qt5::Xml Grantlee5::Templates)
target_link_libraries(kdeinit_khelpcenter KF5::DocTools)
install(TARGETS kdeinit_khelpcenter  ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(TARGETS khelpcenter ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( PROGRAMS org.kde.Help.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES khelpcenter.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khelpcenter.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES khelpcenter.desktop  DESTINATION  ${SHARE_INSTALL_PREFIX}/kde4/services )
install( FILES table-of-contents.xslt glossary.xslt DESTINATION ${DATA_INSTALL_DIR}/khelpcenter )
install( FILES khelpcenterui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/khelpcenter )
install( FILES khelpcenter.categories DESTINATION ${KDE_INSTALL_CONFDIR} )
install( FILES org.kde.Help.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )
install( FILES khelpcenter.categories DESTINATION ${KDE_INSTALL_CONFDIR} )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
