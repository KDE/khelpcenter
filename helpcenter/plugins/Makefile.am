install-data-local: 
	$(mkinstalldirs) $(kde_datadir)
	$(mkinstalldirs) $(kde_datadir)/khelpcenter
	$(mkinstalldirs) $(kde_datadir)/khelpcenter/plugins
	$(mkinstalldirs) $(kde_datadir)/khelpcenter/plugins/Tutorials
	$(INSTALL_DATA) $(srcdir)/Tutorials/.directory $(kde_datadir)/khelpcenter/plugins/Tutorials/.directory
	$(INSTALL_DATA) $(srcdir)/Tutorials/quickstart.kdelnk $(kde_datadir)/khelpcenter/plugins/Tutorials/quickstart.kdelnk

uninstall-local:
	-rm -rf $(mkinstalldirs) $(kde_datadir)/khelpcenter/plugins
