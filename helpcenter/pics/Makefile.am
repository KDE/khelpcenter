
data_DATA = hidetabview.xpm showtabview.xpm
datadir = $(kde_toolbardir)

kde_icon_DATA = khelpcenter.xpm
kde_mini_DATA = helpbook_open.xpm helpdoc.xpm helpdoc.xpm

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-khelpcenter.xpm $(kde_minidir)/khelpcenter.xpm

uninstall-local:
	-rm -f $(kde_minidir)/khelpcenter.xpm

