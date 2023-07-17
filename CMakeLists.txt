cmake_minimum_required(VERSION 3.16)

# KDE Gear version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "23")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

project(KHelpCenter VERSION "6.0.${RELEASE_SERVICE_COMPACT_VERSION}")

set(QT_MIN_VERSION "6.4.0")
set(KF_MIN_VERSION "5.240.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(ECMMarkNonGuiExecutable)
include(ECMOptionalAddSubdirectory)
include(FeatureSummary)
include(CheckIncludeFiles)
include(ECMQtDeclareLoggingCategory)
include(ECMGenerateDBusServiceFile)


find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    DBus
    Widgets
    Xml
    WebEngineWidgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    Bookmarks
    Config
    CoreAddons
    Completion
    DBusAddons
    DocTools
    I18n
    KIO
    XmlGui
    CoreAddons
    Service
    WindowSystem
)

find_package(KF6TextTemplate CONFIG REQUIRED)

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

add_subdirectory( plugins )
add_subdirectory( searchhandlers )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
add_subdirectory( templates )

########### next target ###############

add_executable(khelpcenter)

target_sources(khelpcenter PRIVATE
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
   plugintraverser.cpp
   scrollkeepertreebuilder.cpp
   bookmarkowner.cpp
   searchhandler.cpp

   navigator.h
   navigatoritem.h
   navigatorappitem.h
   navigatorappgroupitem.h
   view.h
   searchwidget.h
   searchengine.h
   docmetainfo.h
   docentrytraverser.h
   grantleeformatter.h
   glossary.h
   toc.h
   mainwindow.h
   docentry.h
   history.h
   application.h
   treebuilder.h
   infotree.h
   plugintraverser.h
   scrollkeepertreebuilder.h
   bookmarkowner.h
   searchhandler.h

   printdoc.cpp
   printdoc.h
   khelpcenter.qrc
)

ecm_qt_declare_logging_category(khelpcenter
    HEADER khc_debug.h
    IDENTIFIER KHC_LOG
    CATEGORY_NAME org.kde.khelpcenter
    DESCRIPTION "khelpcenter"
    EXPORT KHELPCENTER
)

kconfig_add_kcfg_files(khelpcenter prefs.kcfgc )
# kf5_add_app_icon(khelpcenter_SRCS "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/apps/help-browser.png")

target_compile_definitions(khelpcenter PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
target_link_libraries(khelpcenter KF6::Service KF6::DBusAddons KF6::ConfigGui
    KF6::WindowSystem KF6::Bookmarks
    KF6::DocTools Qt::Xml KF6::I18n KF6::XmlGui
    Qt::WebEngineWidgets
    KF6::CoreAddons
    KF6::Completion
    KF6::KIOCore
    KF6::KIOWidgets
)

target_link_libraries(khelpcenter KF6::TextTemplate)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

install(TARGETS khelpcenter ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

ecm_generate_dbus_service_file(
    NAME org.kde.khelpcenter
    EXECUTABLE ${KDE_INSTALL_FULL_BINDIR}/khelpcenter
    DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR}
)

install( PROGRAMS org.kde.khelpcenter.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES khelpcenter.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR} )
install( FILES table-of-contents.xslt glossary.xslt DESTINATION ${KDE_INSTALL_DATADIR}/khelpcenter )
install( FILES org.kde.khelpcenter.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )

ki18n_install(po)
if (QT_MAJOR_VERSION STREQUAL "5")
    kdoctools_install(po)
else()
    MESSAGE(STATUS "docbook needs some fix for building against qt6")
endif()
add_subdirectory( doc )
ecm_qt_install_logging_categories(
    EXPORT KHELPCENTER
    FILE khelpcenter.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
