
add_subdirectory( Applications ) 
if (NOT WIN32)
add_subdirectory( Manpages ) 
add_subdirectory( Scrollkeeper ) 
endif ()


########### install files ###############

install( FILES fundamentals.desktop plasma.desktop 
               onlinehelp.desktop 
               info.desktop kicmodules.desktop 
               kcontrolmodules.desktop 
               konquerorcontrolmodules.desktop
               browsercontrolmodules.desktop 
               filemanagercontrolmodules.desktop
               othercontrolmodules.desktop 
               kioslaves.desktop  DESTINATION  ${KDE_INSTALL_DATADIR}/khelpcenter/plugins )

