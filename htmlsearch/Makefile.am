data_DATA = long.html short.html wrapper.html nomatch.html syntax.html \
		bad_words
datadir = $(kde_datadir)/khelpcenter/en

star_DATA = star.png star_blank.png
stardir = $(kde_datadir)/khelpcenter/pics

INCLUDES = $(all_includes)
METASOURCES = AUTO

lib_LTLIBRARIES = libhtmlsearch.la libkcm_htmlsearch.la

libhtmlsearch_la_SOURCES = htmlsearch.cpp progressdialog.cpp
libhtmlsearch_la_LDFLAGS = $(all_libraries) -module -avoid-version

libkcm_htmlsearch_la_SOURCES = kcmhtmlsearch.cpp 
libkcm_htmlsearch_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_htmlsearch_la_LIBADD = -lkdeui -lkfile

noinst_HEADERS = kcmhtmlsearch.h htmlsearch.h

conf_DATA = htmlsearch.desktop
confdir = $(kde_appsdir)/Settings/Help

bin_PROGRAMS = khtmlindex

khtmlindex_SOURCES = index.cpp
khtmlindex_LDFLAGS = $(KDE_RPATH) $(all_libraries)
khtmlindex_LDADD = libhtmlsearch.la $(LIB_KDEUI)

xpm_DATA = unchecked.xpm checked.xpm
xpmdir = $(kde_datadir)/khelpcenter/pics

#KDE_ICON = sample

messages:
	$(XGETTEXT) $(libhtmlsearch_la_SOURCES) -o $(podir)/htmlsearch.pot
	$(XGETTEXT) $(libkcm_htmlsearch_la_SOURCES) -o $(podir)/kcmhtmlsearch.pot

