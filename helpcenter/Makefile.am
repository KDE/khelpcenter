# this 10 paths are KDE specific. Use them:
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

# Which sources should be compiled for khelpcenter.
khelpcenter_SOURCES = main.cpp toplevel.cpp hhtmlview.cpp \
htreelistitem.cpp htreeview.cpp cgi.cpp info.cpp man.cpp error.cpp dbnew.cpp \
misc.cpp history.cpp booktoken.cpp bookmark.cpp finddlg.cpp options.cpp

khelpcenter_METASOURCES = USE_AUTOMOC

# the library search path. 
khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
khelpcenter_LDADD   = -lkfile -lkfm -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET) \
			-lkhtml -lkimgio -ljpeg -ltiff -lpng -lm -ljscript -lz

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS =

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(khelpcenter_METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(khelpcenter_SOURCES) && mv messages.po ../po/khelpcenter.pot

# just install datas here. Use install-exec-local for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) khelpcenter.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) pics/khelpcenter.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) pics/mini-khelpcenter.xpm $(kde_minidir)/khelpcenter.xpm
	$(mkinstalldirs) $(kde_datadir)
	$(mkinstalldirs) $(kde_datadir)/khelpcenter
	$(mkinstalldirs) $(kde_datadir)/khelpcenter/pics
	
# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/khelpcenter.kdelnk
	-rm -f $(kde_icondir)/khelpcenter.xpm
	-rm -f $(kde_minidir)/khelpcenter.xpm
	-rm -rf $(kde_datadir)/khelpcenter
