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

bin_PROGRAMS = 	kwelcome

kwelcome_SOURCES = kwelcome.cpp kwelcomewidget.cpp main.cpp
kwelcome_METASOURCES = USE_AUTOMOC

kwelcome_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kwelcome_LDADD   = -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)

noinst_HEADERS = kwelcome.h kwelcomewidget.h

DISTCLEANFILES = $(kwelcome_METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kwelcome_SOURCES) && mv messages.po ../po/kwelcome.pot

install-data-local: 
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) pics/kwelcome.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) pics/mini-kwelcome.xpm $(kde_minidir)/kwelcome.xpm
	$(mkinstalldirs) $(kde_datadir)
	$(mkinstalldirs) $(kde_datadir)/kwelcome
	$(mkinstalldirs) $(kde_datadir)/kwelcome/pics
	$(INSTALL_DATA) pics/kdelogo.gif $(kde_datadir)/kwelcome/pics/kdelogo.gif
	$(INSTALL_DATA) pics/kdelogo_big.gif $(kde_datadir)/kwelcome/pics/kdelogo_big.gif
	
uninstall-local:
	-rm -f $(kde_icondir)/kwelcome.xpm
	-rm -f $(kde_minidir)/kwelcome.xpm
	-rm -rf $(kde_datadir)/kwelcome
