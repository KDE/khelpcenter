# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_sounddir      Where system sounds should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

INCLUDES= $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(idldir)

khelpcenter_bin_LDADD   = $(LIB_KFILE) $(LIB_KFM) -lkom $(LIB_KHTML)
khelpcenterclient_LDADD   = $(LIB_KDEUI) -lkom 

SUBDIRS = pics plugins info2html rman

bin_PROGRAMS = 	khelpcenter.bin khelpcenterclient

khelpcenter_bin_SOURCES = main.cpp toplevel.cpp khelpview.cpp \
htreelistitem.cpp htabview.cpp manparser.cpp error.cpp \
finddlg.cpp khelpcenter.cc helpcentercom_impl.cpp \
indexwidget.cpp searchwidget.cpp htmlsearch.cpp mansearch.cpp \
cgiserver.cpp

khelpcenter_bin_METASOURCES = USE_AUTOMOC

khelpcenterclient_SOURCES  = khelpcenterclient.cpp khelpcenter.cc


DISTCLEANFILES = $(khelpcenter_METASOURCES) khelpcenter.cc khelpcenter.h

idl_DATA    =  khelpcenter.idl
idl_HEADERS =  khelpcenter.h

BUILT_SOURCES = khelpcenter.h

khelpcenter.h: $(srcdir)/khelpcenter.idl
	rm -rf khelpcenter.h
	$(IDL) $(IDL_FLAGS) $(srcdir)/khelpcenter.idl

khelpcenter.cc: $(srcdir)/khelpcenter.idl
	rm -rf khelpcenter.cc
	$(IDL) $(IDL_FLAGS) $(srcdir)/khelpcenter.idl

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(khelpcenter_bin_SOURCES) && mv messages.po ../po/khelpcenter.pot

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/khelpcenter $(bindir)/khelpcenter

uninstall-local:
	-rm -f $(bindir)/khelpcenter

