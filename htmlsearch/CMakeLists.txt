


########### next target ###############

set(htmlsearch_LIB_SRCS htmlsearch.cpp progressdialog.cpp )


kde4_add_library(htmlsearch SHARED ${htmlsearch_LIB_SRCS})

target_link_libraries(htmlsearch  ${KDE4_KIO_LIBS} )

set_target_properties(htmlsearch PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS htmlsearch  DESTINATION ${LIB_INSTALL_DIR} )


########### next target ###############

set(kcm_htmlsearch_LIB_SRCS kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp )


kde4_add_library(kcm_htmlsearch SHARED ${kcm_htmlsearch_LIB_SRCS})

target_link_libraries(kcm_htmlsearch  ${KDE4_KIO_LIBS} )

set_target_properties(kcm_htmlsearch PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS kcm_htmlsearch  DESTINATION ${LIB_INSTALL_DIR} )


########### next target ###############

set(khtmlindex_SRCS index.cpp )


kde4_add_executable(khtmlindex ${khtmlindex_SRCS})

target_link_libraries(khtmlindex  ${KDE4_KDEUI_LIBS} htmlsearch )

install(PROGRAMS khtmlindex DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES htmlsearch.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES long.html short.html wrapper.html nomatch.html syntax.html   DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/en )
install( FILES star.png star_blank.png  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/pics )
install( FILES unchecked.xpm checked.xpm  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/pics )

install( PROGRAMS meinproc_wrapper  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/ )

kde4_install_icons( ${DATA_INSTALL_DIR}/khelpcenter/  )



