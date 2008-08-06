
add_subdirectory( Applications ) 
add_subdirectory( Manpages ) 
add_subdirectory( Tutorials ) 
add_subdirectory( Scrollkeeper ) 



########### install files ###############

install( FILES plasma.desktop contact.desktop faq.desktop info.desktop    links.desktop support.desktop userguide.desktop welcome.desktop    kicmodules.desktop kcontrolmodules.desktop    kioslaves.desktop  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/plugins )

