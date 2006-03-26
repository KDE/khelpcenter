kde4_header()
include_directories(  ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDES} )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khelpcenter/plugins/Manpages FILES .directory man1.desktop man2.desktop man3.desktop man4.desktop    man5.desktop man6.desktop man7.desktop man8.desktop man9.desktop mann.desktop  )

kde4_footer()



#original Makefile.am contents follow:

#
#ManPages_DATA = .directory man1.desktop man2.desktop man3.desktop man4.desktop \
#  man5.desktop man6.desktop man7.desktop man8.desktop man9.desktop mann.desktop 
#ManPagesdir = $(kde_datadir)/khelpcenter/plugins/Manpages
