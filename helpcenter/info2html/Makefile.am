imagedir=$(kde_datadir)/khelpcenter/gif

bindir = $(kde_cgidir)
bin_SCRIPTS = info2html

data_DATA = info2html.conf
datadir = $(kde_cgidir)

install-data-local: 
	$(mkinstalldirs) $(kde_datadir)/khelpcenter
	$(mkinstalldirs) $(kde_datadir)/khelpcenter/gif
	$(INSTALL_DATA) gif/l_arrow.gif $(imagedir)/l_arrow.gif
	$(INSTALL_DATA) gif/r_arrow.gif $(imagedir)/r_arrow.gif
	$(INSTALL_DATA) gif/u_arrow.gif $(imagedir)/u_arrow.gif
	$(INSTALL_DATA) gif/r_hand.gif $(imagedir)/r_hand.gif
