

INCLUDES= $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(idldir)

khelpcenterd_LDADD   = $(LIB_KDEUI) $(LIB_KFILE) -lkom $(LIB_KHTML)
khelpcenter_LDADD   = -lkom -lkded

SUBDIRS = . kwelcome kwid pics plugins \
	info2html rman

bin_PROGRAMS = 	khelpcenterd khelpcenter

khelpcenterd_SOURCES = main.cpp toplevel.cpp khelpview.cpp \
	htreelistitem.cpp htabview.cpp man.cpp \
	finddlg.cpp khelpcenter.cc khelpcenter_impl.cpp \
	indexwidget.cpp searchwidget.cpp htmlsearch.cpp mansearch.cpp \
	cgiserver.cpp

khelpcenterd_METASOURCES = USE_AUTOMOC

khelpcenter_SOURCES  = khelpcenterclient.cpp khelpcenter.idl


idl_DATA    =  khelpcenter.idl

main.cpp: khelpcenter.h

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.h *.cpp && mv messages.po ../../po/khelpcenter.pot
	for subdir in $(SUBDIRS); do \
	   echo "Making messages in $$subdir"; \
	   (cd $$subdir && $(MAKE) -f Makefile.am messages) || exit 1; \
	done

