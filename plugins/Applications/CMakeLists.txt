kde4_header()



########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khelpcenter/plugins/Applications FILES .directory )

kde4_footer()



#original Makefile.am contents follow:

#
#applications_DATA = .directory
#applicationsdir = $(kde_datadir)/khelpcenter/plugins/Applications
