


########### next target ###############

set(htmlsearch_LIB_SRCS htmlsearch.cpp progressdialog.cpp )

kde4_automoc(${htmlsearch_LIB_SRCS})

kde4_add_library(htmlsearch SHARED ${htmlsearch_LIB_SRCS})

target_link_libraries(htmlsearch  ${KDE4_KIO_LIBS} )

set_target_properties(htmlsearch PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS htmlsearch  DESTINATION ${LIB_INSTALL_DIR} )


########### next target ###############

set(kcm_htmlsearch_LIB_SRCS kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp )

kde4_automoc(${kcm_htmlsearch_LIB_SRCS})

kde4_add_library(kcm_htmlsearch SHARED ${kcm_htmlsearch_LIB_SRCS})

target_link_libraries(kcm_htmlsearch  ${KDE4_KIO_LIBS} )

set_target_properties(kcm_htmlsearch PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS kcm_htmlsearch  DESTINATION ${LIB_INSTALL_DIR} )


########### next target ###############

set(khtmlindex_SRCS index.cpp )

kde4_automoc(${khtmlindex_SRCS})

kde4_add_executable(khtmlindex ${khtmlindex_SRCS})

target_link_libraries(khtmlindex  ${KDE4_KDEUI_LIBS} htmlsearch )

install(TARGETS khtmlindex DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES htmlsearch.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES long.html short.html wrapper.html nomatch.html syntax.html   DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/en )
install( FILES star.png star_blank.png  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/pics )
install( FILES unchecked.xpm checked.xpm  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/pics )

install( FILES meinproc_wrapper  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/ )

kde4_install_icons( ${DATA_INSTALL_DIR}/khelpcenter/  )




#original Makefile.am contents follow:

#khelpcenter_en_data_DATA = long.html short.html wrapper.html nomatch.html syntax.html 
#khelpcenter_en_datadir = $(kde_datadir)/khelpcenter/en
#
#star_DATA = star.png star_blank.png
#stardir = $(kde_datadir)/khelpcenter/pics
#
#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#
#lib_LTLIBRARIES = libhtmlsearch.la libkcm_htmlsearch.la
#
#libhtmlsearch_la_SOURCES = htmlsearch.cpp progressdialog.cpp
#libhtmlsearch_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libhtmlsearch_la_LIBADD = $(LIB_KIO)
#
#libkcm_htmlsearch_la_SOURCES = kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp
#libkcm_htmlsearch_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
#libkcm_htmlsearch_la_LIBADD = $(LIB_KIO)
#
#noinst_HEADERS = kcmhtmlsearch.h htmlsearch.h
#
#xdg_apps_DATA = htmlsearch.desktop
#
#bin_PROGRAMS = khtmlindex
#
#wrapperdir = $(kde_datadir)/khelpcenter/
#wrapper_SCRIPTS = meinproc_wrapper
#
#khtmlindex_SOURCES = index.cpp
#khtmlindex_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#khtmlindex_LDADD = libhtmlsearch.la $(LIB_KDEUI)
#
#xpm_DATA = unchecked.xpm checked.xpm
#xpmdir = $(kde_datadir)/khelpcenter/pics
#
#### TODO: why do we need 2 POT files for one directory?
#messages:
#	$(XGETTEXT) index.cpp tmlsearch.cpp progressdialog.cpp -o $(podir)/htmlsearch.pot
#	$(XGETTEXT) kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp -o $(podir)/kcmhtmlsearch.pot
#
#KDE_ICON=AUTO
