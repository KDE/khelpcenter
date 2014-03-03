

########### next target ###############

set(htmlsearch_LIB_SRCS htmlsearch.cpp progressdialog.cpp)


kde4_add_library(htmlsearch SHARED ${htmlsearch_LIB_SRCS})

target_link_libraries(htmlsearch  ${KDE4_KIO_LIBS})

set_target_properties(htmlsearch PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
install(TARGETS htmlsearch ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(kcm_htmlsearch_LIB_SRCS kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp)


kde4_add_library(kcm_htmlsearch SHARED ${kcm_htmlsearch_LIB_SRCS})

target_link_libraries(kcm_htmlsearch  ${KDE4_KIO_LIBS})

set_target_properties(kcm_htmlsearch PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
install(TARGETS kcm_htmlsearch ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

add_executable(khtmlindex index.cpp)

target_link_libraries(khtmlindex  ${KDE4_KDEUI_LIBS} htmlsearch )

install(PROGRAMS khtmlindex ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES htmlsearch.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES long.html short.html wrapper.html nomatch.html syntax.html   DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/en )
install( FILES star.png star_blank.png  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/pics )
install( FILES unchecked.xpm checked.xpm  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/pics )

install( PROGRAMS meinproc_wrapper  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/ )

kde4_install_icons( ${DATA_INSTALL_DIR}/khelpcenter/  )
