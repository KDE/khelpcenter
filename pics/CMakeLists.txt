kde4_header()



########### install files ###############

#install_files( ${DATA_INSTALL_DIR}/khelpcenter/icons FILES helpdoc.png helpbook.png helpbook_open.png )

kde4_install_icons( ${ICON_INSTALL_DIR} crystalsvg )

kde4_footer()



#original Makefile.am contents follow:

#
#KDE_ICON = khelpcenter
#
#khelpcenter_ICON = helpdoc helpbook helpbook_open
#khelpcenterdir = $(kde_datadir)/khelpcenter/icons
#
