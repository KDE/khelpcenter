kde4_header()
include_directories(  ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDES} )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khelpcenter/plugins/Scrollkeeper FILES .directory scrollkeeper.desktop )

kde4_footer()



#original Makefile.am contents follow:

#
#applications_DATA = .directory scrollkeeper.desktop
#applicationsdir = $(kde_datadir)/khelpcenter/plugins/Scrollkeeper
