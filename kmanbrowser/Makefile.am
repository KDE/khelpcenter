
INCLUDES= -I../libkhc -I$(srcdir)/../libkhc $(all_includes) $(MICO_INCLUDES) 
LDFLAGS = $(all_libraries) $(KDE_RPATH) $(MICO_LDFLAGS)
IDL_FLAGS = --relative-paths --any -I$(idldir)

bin_PROGRAMS = 	kmanbrowser

kmanbrowser_LDADD  = -lkparts $(LIB_KDEUI) $(LIB_KFILE) $(LIB_KHTML) $(top_builddir)/khelpcenter/libkhc/libkhc.la


kmanbrowser_SOURCES = kmanbrowser.idl kmb_main.cc \
	kmb_mainwindow.cc kmb_view.cc

kmanbrowser_METASOURCES = kmb_mainwindow.moc kmb_view.moc

noinst_HEADERS = kmanbrowser.h kmb_mainwindow.h kmb_view.h

browser_IDLDIR = $(idldir)

kmanbrowser.idl: $(idldir)/browser.idl 
