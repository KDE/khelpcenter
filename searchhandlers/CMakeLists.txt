include_directories(
  ${XAPIAN_INCLUDE_DIR}
)

# Xapian does not like signals/slots #define's
add_definitions(-DQT_NO_KEYWORDS)

# Xapian indexer
set(khc_xapianindexer_SOURCES
   cachereader.cpp
   htmltextdump.cpp
   xapiancommon.cpp
   xapianindexer.cpp
)
add_executable(khc_xapianindexer ${khc_xapianindexer_SOURCES})
kde_target_enable_exceptions(khc_xapianindexer PRIVATE)
ecm_mark_nongui_executable(khc_xapianindexer)
target_link_libraries(khc_xapianindexer KF5::DocTools Qt5::Core KF5::Archive KF5::CoreAddons ${XAPIAN_LIBRARIES} PkgConfig::gumbo)
install(TARGETS khc_xapianindexer DESTINATION ${LIBEXEC_INSTALL_DIR})

# Xapian search
set(khc_xapiansearch_SOURCES
   xapiancommon.cpp
   xapiansearch.cpp
)
add_executable(khc_xapiansearch ${khc_xapiansearch_SOURCES})
kde_target_enable_exceptions(khc_xapiansearch PRIVATE)
ecm_mark_nongui_executable(khc_xapiansearch)
target_link_libraries(khc_xapiansearch Qt5::Core ${XAPIAN_LIBRARIES})
install(TARGETS khc_xapiansearch DESTINATION ${LIBEXEC_INSTALL_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/xapian.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/xapian.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xapian.desktop DESTINATION ${DATA_INSTALL_DIR}/khelpcenter/searchhandlers)

if (NOT WIN32)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man.desktop.cmake
${CMAKE_CURRENT_BINARY_DIR}/man.desktop )

########### install files ###############

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/man.desktop DESTINATION ${DATA_INSTALL_DIR}/khelpcenter/searchhandlers)

install(PROGRAMS khc_mansearch.pl DESTINATION ${LIBEXEC_INSTALL_DIR})

endif ()
