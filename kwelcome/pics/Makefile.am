
kde_icon_DATA = kwelcome.xpm

pics_DATA = kdelogo.gif kdelogo_big.gif
picsdir = $(kde_datadir)/kwelcome/pics

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kwelcome.xpm $(kde_minidir)/kwelcome.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kwelcome.xpm

