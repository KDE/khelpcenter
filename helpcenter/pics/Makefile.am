data_DATA = hidetree.xpm showtree.xpm
datadir = $(kde_toolbardir)

kde_icon_DATA = khelpcenter.xpm

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-khelpcenter.xpm $(kde_minidir)/khelpcenter.xpm
uninstall-local:
	-rm -f $(kde_minidir)/khelpcenter.xpm
