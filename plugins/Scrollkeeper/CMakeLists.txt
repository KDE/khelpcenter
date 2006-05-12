kde4_header()



########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khelpcenter/plugins/Scrollkeeper FILES .directory scrollkeeper.desktop )

kde4_footer()



#original Makefile.am contents follow:

#
#applications_DATA = .directory scrollkeeper.desktop
#applicationsdir = $(kde_datadir)/khelpcenter/plugins/Scrollkeeper
