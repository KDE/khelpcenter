
INCLUDES= -I../libkhc -I$(srcdir)/../libkhc  $(all_includes) $(MICO_INCLUDES)
LDFLAGS = $(all_libraries) $(KDE_RPATH) $(MICO_LDFLAGS) -lcutesti
IDL_FLAGS = --relative-paths --any -I$(idldir)

bin_PROGRAMS = kinfobrowser
data_DATA = kde-info2html kde-info2html.conf
datadir = $(kde_datadir)/kinfobrowser

SUBDIRS = pics

kinfobrowser_LDADD  = -lkparts $(LIB_KDEUI) $(LIB_KFILE) $(LIB_KHTML) $(top_builddir)/khelpcenter/libkhc/libkhc.la


kinfobrowser_SOURCES = kinfobrowser.idl kib_main.cc kib_application.cc \
	kib_mainwindow.cc kib_view.cc

kinfobrowser_METASOURCES = kib_mainwindow.moc kib_view.moc

noinst_HEADERS = kinfobrowser.h kib_mainwindow.h kib_application.h kib_view.h

browser_IDLDIR = $(idldir)

kinfobrowser.idl: $(idldir)/browser.idl

messages:
	$(XGETTEXT) -C -ktranslate -kaliasLocal -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/kinfobrowser.pot 