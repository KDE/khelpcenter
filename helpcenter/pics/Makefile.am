data_DATA = hidetabview.xpm showtabview.xpm
datadir = $(kde_toolbardir)

kde_icon_DATA = khelpcenter.xpm

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-khelpcenter.xpm $(kde_minidir)/khelpcenter.xpm
	$(INSTALL_DATA) $(srcdir)/helpbook.xpm $(kde_minidir)/helpbook.xpm
	$(INSTALL_DATA) $(srcdir)/helpdoc.xpm $(kde_minidir)/helpdoc.xpm
uninstall-local:
	-rm -f $(kde_minidir)/khelpcenter.xpm
	-rm -f $(kde_minidir)/helpbook.xpm
	-rm -f $(kde_minidir)/helpdoc.xpm
