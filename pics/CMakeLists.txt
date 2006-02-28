kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khelpcenter/icons FILES  )

kde4_install_icons( hicolor crystalsvg )

kde4_footer()



#original Makefile.am contents follow:

#
#KDE_ICON = khelpcenter
#
#khelpcenter_ICON = helpdoc helpbook helpbook_open
#khelpcenterdir = $(kde_datadir)/khelpcenter/icons
#
