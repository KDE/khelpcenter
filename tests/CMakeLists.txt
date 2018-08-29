set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

########### next target ###############

set(testmetainfo_SRCS
    testmetainfo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docmetainfo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docentry.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docentrytraverser.cpp )
ecm_qt_declare_logging_category(testmetainfo_SRCS HEADER khc_debug.h IDENTIFIER KHC_LOG CATEGORY_NAME org.kde.khelpcenter)

kconfig_add_kcfg_files(testmetainfo_SRCS ../prefs.kcfgc)

add_executable(testmetainfo ${testmetainfo_SRCS})
ecm_mark_as_test(testmetainfo)

target_link_libraries(testmetainfo KF5::ConfigGui KF5::CoreAddons KF5::I18n)

