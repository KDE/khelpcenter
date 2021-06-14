set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

########### next target ###############

add_executable(testmetainfo)
ecm_mark_as_test(testmetainfo)

target_sources(testmetainfo PRIVATE
    testmetainfo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docmetainfo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docentry.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../docentrytraverser.cpp )
ecm_qt_declare_logging_category(testmetainfo
    HEADER khc_debug.h
    IDENTIFIER KHC_LOG
    CATEGORY_NAME org.kde.khelpcenter
)

kconfig_add_kcfg_files(testmetainfo ../prefs.kcfgc)

target_include_directories(testmetainfo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(testmetainfo KF5::ConfigGui KF5::CoreAddons KF5::I18n)

