
add_subdirectory( Applications ) 
add_subdirectory( Manpages ) 
add_subdirectory( Tutorials ) 
add_subdirectory( Scrollkeeper ) 



########### install files ###############

install( FILES applets.desktop contact.desktop faq.desktop info.desktop    links.desktop support.desktop userguide.desktop welcome.desktop    kicmodules.desktop kcontrolmodules.desktop konqplugins.desktop    kioslaves.desktop  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/plugins )




#original Makefile.am contents follow:

#
#SUBDIRS = Applications Manpages Tutorials Scrollkeeper
#
#plugin_DATA = applets.desktop contact.desktop faq.desktop info.desktop \
#  links.desktop support.desktop userguide.desktop welcome.desktop \
#  kicmodules.desktop kcontrolmodules.desktop konqplugins.desktop \
#  kioslaves.desktop
#
#plugindir = $(kde_datadir)/khelpcenter/plugins
