
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/khc_docbookdig.pl.cmake ${CMAKE_CURRENT_BINARY_DIR}/khc_docbookdig.pl @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/khc_htdig.pl.cmake ${CMAKE_CURRENT_BINARY_DIR}/khc_htdig.pl @ONLY )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docbook.desktop.cmake
${CMAKE_CURRENT_BINARY_DIR}/docbook.desktop )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man.desktop.cmake
${CMAKE_CURRENT_BINARY_DIR}/man.desktop )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/htdig.desktop.cmake
${CMAKE_CURRENT_BINARY_DIR}/htdig.desktop )

macro_additional_clean_files( ${CMAKE_CURRENT_BINARY_DIR}/khc_htdig.pl {CMAKE_CURRENT_BINARY_DIR}/khc_docbookdig.pl @ONLY)

########### install files ###############

install( FILES htdig_long.html  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/searchhandlers/htdig )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/htdig.desktop ${CMAKE_CURRENT_BINARY_DIR}/man.desktop ${CMAKE_CURRENT_BINARY_DIR}/docbook.desktop  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/searchhandlers )

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/khc_htdig.pl khc_htsearch.pl khc_mansearch.pl ${CMAKE_CURRENT_BINARY_DIR}/khc_docbookdig.pl DESTINATION ${LIBEXEC_INSTALL_DIR})

