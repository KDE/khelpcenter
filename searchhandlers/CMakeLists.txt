
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/khc_docbookdig.pl.cmake ${CMAKE_CURRENT_BINARY_DIR}/khc_docbookdig.pl @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/khc_htdig.pl.cmake ${CMAKE_CURRENT_BINARY_DIR}/khc_htdig.pl @ONLY )

macro_additional_clean_files( ${CMAKE_CURRENT_BINARY_DIR}/khc_htdig.pl {CMAKE_CURRENT_BINARY_DIR}/khc_docbookdig.pl @ONLY)

########### install files ###############

install( FILES htdig_long.html  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/searchhandlers/htdig )
install( FILES htdig.desktop man.desktop docbook.desktop  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/searchhandlers )

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/khc_htdig.pl khc_htsearch.pl khc_mansearch.pl ${CMAKE_CURRENT_BINARY_DIR}/khc_docbookdig.pl DESTINATION ${BIN_INSTALL_DIR})



#original Makefile.am contents follow:

#searchhandlers_DATA = htdig.desktop man.desktop docbook.desktop
#
#searchhandlersdir = $(kde_datadir)/khelpcenter/searchhandlers
#
#kde_bin_SCRIPTS = khc_htdig.pl khc_htsearch.pl khc_mansearch.pl \
#  khc_docbookdig.pl
#
#htdigdata_DATA = htdig_long.html
#
#htdigdatadir = $(kde_datadir)/khelpcenter/searchhandlers/htdig
