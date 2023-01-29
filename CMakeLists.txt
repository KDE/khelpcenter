cmake_minimum_required(VERSION 3.16)

# KDE Gear version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "22")
set (RELEASE_SERVICE_VERSION_MINOR "12")
set (RELEASE_SERVICE_VERSION_MICRO "2")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

project(KHelpCenter VERSION "5.8.${RELEASE_SERVICE_COMPACT_VERSION}")

set(QT_MIN_VERSION "5.15.0")
set(KF_MIN_VERSION "5.89.0")

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

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    DBus
    Widgets
    Xml
)

find_package(KF5 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Archive
    Bookmarks
    Config
    CoreAddons
    DBusAddons
    DocTools
    I18n
    KHtml
    Service
    WindowSystem
)

find_package(Grantlee5 REQUIRED)
set_package_properties(Grantlee5 PROPERTIES
    DESCRIPTION "Support for HTML templates"
    URL "https://github.com/steveire/grantlee"
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

add_subdirectory( plugins )
add_subdirectory( searchhandlers )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
add_subdirectory( doc )
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
   fontdialog.cpp
   plugintraverser.cpp
   scrollkeepertreebuilder.cpp
   bookmarkowner.cpp
   searchhandler.cpp

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
target_link_libraries(khelpcenter KF5::KHtml KF5::Service KF5::DBusAddons KF5::ConfigGui KF5::WindowSystem KF5::Bookmarks KF5::DocTools Qt5::Xml Grantlee5::Templates)

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
install( FILES khelpcenter.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )
install( FILES khelpcenter.desktop  DESTINATION  ${KDE_INSTALL_DATAROOTDIR}/kde4/services )
install( FILES table-of-contents.xslt glossary.xslt DESTINATION ${KDE_INSTALL_DATADIR}/khelpcenter )
install( FILES org.kde.khelpcenter.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )

ki18n_install(po)
kdoctools_install(po)

ecm_qt_install_logging_categories(
    EXPORT KHELPCENTER
    FILE khelpcenter.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
