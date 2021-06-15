include_directories(
  ${XAPIAN_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
)

# Xapian does not like signals/slots #define's
add_definitions(-DQT_NO_KEYWORDS)

# Xapian indexer
add_executable(khc_xapianindexer)
kde_target_enable_exceptions(khc_xapianindexer PRIVATE)
ecm_mark_nongui_executable(khc_xapianindexer)

target_sources(khc_xapianindexer PRIVATE
   cachereader.cpp
   htmltextdump.cpp
   xapiancommon.cpp
   xapianindexer.cpp
)

target_link_libraries(khc_xapianindexer KF5::DocTools Qt5::Core KF5::Archive KF5::CoreAddons ${XAPIAN_LIBRARIES} ${LIBXML2_LIBRARIES})
install(TARGETS khc_xapianindexer DESTINATION ${KDE_INSTALL_LIBEXECDIR})

# Xapian search
add_executable(khc_xapiansearch)
kde_target_enable_exceptions(khc_xapiansearch PRIVATE)
ecm_mark_nongui_executable(khc_xapiansearch)

target_sources(khc_xapiansearch PRIVATE
   xapiancommon.cpp
   xapiansearch.cpp
)
target_link_libraries(khc_xapiansearch Qt5::Core ${XAPIAN_LIBRARIES})
install(TARGETS khc_xapiansearch DESTINATION ${KDE_INSTALL_LIBEXECDIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/xapian.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/xapian.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xapian.desktop DESTINATION ${KDE_INSTALL_DATADIR}/khelpcenter/searchhandlers)

if (NOT WIN32)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man.desktop.cmake
${CMAKE_CURRENT_BINARY_DIR}/man.desktop )

########### install files ###############

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/man.desktop DESTINATION ${KDE_INSTALL_DATADIR}/khelpcenter/searchhandlers)

install(PROGRAMS khc_mansearch.pl DESTINATION ${KDE_INSTALL_LIBEXECDIR})

endif ()
