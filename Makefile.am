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

khelpcenterd_LDADD   = $(LIB_KDEUI) $(LIB_KFILE) -lkom $(LIB_KHTML)
khelpcenter_LDADD   = -lkom -lkded

SUBDIRS = . kwelcome kwid pics plugins \
	info2html rman

bin_PROGRAMS = 	khelpcenterd khelpcenter

khelpcenterd_SOURCES = main.cpp toplevel.cpp khelpview.cpp \
htreelistitem.cpp htabview.cpp man.cpp \
finddlg.cpp khelpcenter.cc khelpcenter_impl.cpp \
indexwidget.cpp searchwidget.cpp htmlsearch.cpp mansearch.cpp \
cgiserver.cpp

khelpcenterd_METASOURCES = USE_AUTOMOC

khelpcenter_SOURCES  = khelpcenterclient.cpp khelpcenter.cc


DISTCLEANFILES = $(khelpcenter_METASOURCES) khelpcenter.cc khelpcenter.h

idl_DATA    =  khelpcenter.idl
idl_HEADERS =  khelpcenter.h

khelpcenter.cc: khelpcenter.h
khelpcenter.h: $(srcdir)/khelpcenter.idl $(micodir)/include/CORBA.h
	rm -rf khelpcenter.h
	$(IDL) $(IDL_FLAGS) $(srcdir)/khelpcenter.idl

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.h *.cpp && mv messages.po ../../po/khelpcenter.pot
	for subdir in $(SUBDIRS); do \
	   echo "Making messages in $$subdir"; \
	   (cd $$subdir && $(MAKE) -f Makefile.am messages) || exit 1; \
	done
