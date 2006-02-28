kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khelpcenter/plugins/Tutorials FILES .directory quickstart.desktop visualdict.desktop )

kde4_footer()



#original Makefile.am contents follow:

#
#tut_DATA = .directory quickstart.desktop visualdict.desktop
#tutdir = $(kde_datadir)/khelpcenter/plugins/Tutorials
#
#
