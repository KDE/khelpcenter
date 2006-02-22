khelpcenter_en_data_DATA = long.html short.html wrapper.html nomatch.html syntax.html 
khelpcenter_en_datadir = $(kde_datadir)/khelpcenter/en

star_DATA = star.png star_blank.png
stardir = $(kde_datadir)/khelpcenter/pics

INCLUDES = $(all_includes)
METASOURCES = AUTO

lib_LTLIBRARIES = libhtmlsearch.la libkcm_htmlsearch.la

libhtmlsearch_la_SOURCES = htmlsearch.cpp progressdialog.cpp
libhtmlsearch_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libhtmlsearch_la_LIBADD = $(LIB_KIO)

libkcm_htmlsearch_la_SOURCES = kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp
libkcm_htmlsearch_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_htmlsearch_la_LIBADD = $(LIB_KIO)

noinst_HEADERS = kcmhtmlsearch.h htmlsearch.h

xdg_apps_DATA = htmlsearch.desktop

bin_PROGRAMS = khtmlindex

wrapperdir = $(kde_datadir)/khelpcenter/
wrapper_SCRIPTS = meinproc_wrapper

khtmlindex_SOURCES = index.cpp
khtmlindex_LDFLAGS = $(KDE_RPATH) $(all_libraries)
khtmlindex_LDADD = libhtmlsearch.la $(LIB_KDEUI)

xpm_DATA = unchecked.xpm checked.xpm
xpmdir = $(kde_datadir)/khelpcenter/pics

### TODO: why do we need 2 POT files for one directory?
messages:
	$(XGETTEXT) index.cpp tmlsearch.cpp progressdialog.cpp -o $(podir)/htmlsearch.pot
	$(XGETTEXT) kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp -o $(podir)/kcmhtmlsearch.pot

KDE_ICON=AUTO
