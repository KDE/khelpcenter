
INCLUDES= -I../libkhc -I$(srcdir)/../libkhc $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)

bin_PROGRAMS = kmanbrowser

kmanbrowser_LDADD  = $(LIB_KHTML) $(LIB_KPARTS) $(LIB_KDEUI) $(LIB_KFILE) $(top_builddir)/khelpcenter/libkhc/libkhc.la

kmanbrowser_SOURCES = kmb_main.cc kmb_mainwindow.cc kmb_view.cc man.cpp

kmanbrowser_METASOURCES = kmb_mainwindow.moc kmb_view.moc

noinst_HEADERS = kmb_mainwindow.h kmb_view.h man.h

data_DATA = kmanbrowser.rc
datadir = $(kde_datadir)/kmanbrowser

messages:
	$(XGETTEXT) -kaliasLocal *.cc *.h -o $(podir)/kmanbrowser.pot 	
