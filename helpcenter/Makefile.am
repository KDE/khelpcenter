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

APPSDIR = $(kde_appsdir)
INCLUDES= $(all_includes)

SUBDIRS = pics

bin_PROGRAMS = 	khelpcenter

khelpcenter_SOURCES = main.cpp toplevel.cpp hhtmlview.cpp \
htreelistitem.cpp htreeview.cpp cgi.cpp info.cpp man.cpp error.cpp dbnew.cpp \
misc.cpp history.cpp booktoken.cpp bookmark.cpp finddlg.cpp options.cpp

khelpcenter_METASOURCES = USE_AUTOMOC

khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)

khelpcenter_LDADD   = $(LIB_KFILE) $(LIB_KHTMLW)

DISTCLEANFILES = $(khelpcenter_METASOURCES)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(khelpcenter_SOURCES) && mv messages.po ../po/khelpcenter.pot

install-data-local: 
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) pics/khelpcenter.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) pics/mini-khelpcenter.xpm $(kde_minidir)/khelpcenter.xpm
	$(mkinstalldirs) $(kde_datadir)
	$(mkinstalldirs) $(kde_datadir)/khelpcenter
	$(mkinstalldirs) $(kde_datadir)/khelpcenter/pics
	
# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(kde_icondir)/khelpcenter.xpm
	-rm -f $(kde_minidir)/khelpcenter.xpm
	-rm -rf $(kde_datadir)/khelpcenter
