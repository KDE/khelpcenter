SUBDIRS = pics

INCLUDES= -I$(srcdir)/../libkhc $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)

bin_PROGRAMS = kinfobrowser

kinfobrowser_LDADD =  $(LIB_KHTML) $(LIB_KHTML) $(LIB_KFILE) $(top_builddir)/khelpcenter/libkhc/libkhc.la

kinfobrowser_SOURCES = kib_main.cc kib_mainwindow.cc kib_view.cc

kinfobrowser_METASOURCES = kib_mainwindow.moc kib_view.moc

noinst_HEADERS = kib_mainwindow.h kib_view.h

data_DATA = kde-info2html kde-info2html.conf kinfobrowser.rc
datadir = $(kde_datadir)/kinfobrowser

messages:
	$(XGETTEXT) -kaliasLocal *.cc *.h -o $(podir)/kinfobrowser.pot 
