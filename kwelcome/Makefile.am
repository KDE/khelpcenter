INCLUDES= $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD   = $(LIB_KDEUI)

bin_PROGRAMS = kwelcome

kwelcome_SOURCES = kwelcome.cc main.cc

kwelcome_METASOURCES = kwelcome.moc

noinst_HEADERS = kwelcome.h

pics_DATA = welcome.png
picsdir = $(kde_datadir)/kwelcome/pics

messages:
	$(XGETTEXT) -kaliasLocal *.h *.cc -o $(podir)/kwelcome.pot
