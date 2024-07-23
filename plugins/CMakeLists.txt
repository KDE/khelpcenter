
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
                kioworkers.desktop  DESTINATION  ${KDE_INSTALL_DATADIR}/khelpcenter/plugins )

