kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khelpcenter/searchhandlers/htdig FILES htdig_long.html )
install_files( ${DATA_INSTALL_DIR}/khelpcenter/searchhandlers FILES htdig.desktop man.desktop docbook.desktop )

kde4_footer()



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
