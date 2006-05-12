kde4_header()



########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khelpcenter/plugins/Tutorials FILES .directory quickstart.desktop visualdict.desktop )

kde4_footer()



#original Makefile.am contents follow:

#
#tut_DATA = .directory quickstart.desktop visualdict.desktop
#tutdir = $(kde_datadir)/khelpcenter/plugins/Tutorials
#
#
