data_DATA = hidetabview.xpm showtabview.xpm
datadir = $(kde_toolbardir)

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) $(srcdir)/khelpcenter.xpm $(kde_icondir)/khelpcenter.xpm
	$(INSTALL_DATA) $(srcdir)/helpbook.xpm $(kde_minidir)/helpbook.xpm
	$(INSTALL_DATA) $(srcdir)/helpbook_open.xpm $(kde_minidir)/helpbook_open.xpm
	$(INSTALL_DATA) $(srcdir)/helpdoc.xpm $(kde_minidir)/helpdoc.xpm 
uninstall-local:
	-rm -f $(kde_icondir)/khelpcenter.xpm
	-rm -f $(kde_minidir)/khelpcenter.xpm
	-rm -f $(kde_minidir)/helpbook.xpm
	-rm -f $(kde_minidir)/helpbook_open.xpm
	-rm -f $(kde_minidir)/helpdoc.xpm
