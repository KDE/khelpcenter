kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(htmlsearch_LIB_SRCS htmlsearch.cpp progressdialog.cpp )

kde4_automoc(${htmlsearch_LIB_SRCS})

kde4_add_library(htmlsearch SHARED ${htmlsearch_LIB_SRCS})

target_link_libraries(htmlsearch  ${QT_AND_KDECORE_LIBS} kio )

set_target_properties(htmlsearch PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} htmlsearch )


########### next target ###############

set(kcm_htmlsearch_LIB_SRCS kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp )

kde4_automoc(${kcm_htmlsearch_LIB_SRCS})

kde4_add_library(kcm_htmlsearch SHARED ${kcm_htmlsearch_LIB_SRCS})

target_link_libraries(kcm_htmlsearch  ${QT_AND_KDECORE_LIBS} kio )

set_target_properties(kcm_htmlsearch PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kcm_htmlsearch )


########### next target ###############

set(khtmlindex_SRCS index.cpp )

kde4_automoc(${khtmlindex_SRCS})

kde4_add_executable(khtmlindex ${khtmlindex_SRCS})

target_link_libraries(khtmlindex  ${QT_AND_KDECORE_LIBS} htmlsearch kdeui )

install_targets(/bin khtmlindex )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES htmlsearch.desktop )
install_files( ${DATA_INSTALL_DIR}/khelpcenter/ FILES  )
install_files( ${DATA_INSTALL_DIR}/khelpcenter/en FILES long.html short.html wrapper.html nomatch.html syntax.html  )
install_files( ${DATA_INSTALL_DIR}/khelpcenter/pics FILES star.png star_blank.png )
install_files( ${DATA_INSTALL_DIR}/khelpcenter/pics FILES unchecked.xpm checked.xpm )

kde4_install_icons( ${ICON_INSTALL_DIR} crystalsvg )

kde4_footer()



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
