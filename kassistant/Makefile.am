
INCLUDES= $(all_includes) $(MICO_INCLUDES) 
LDFLAGS = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD   = -lkom $(LIB_KHTML) $(LIB_KDEUI) $(LIBMICO) -ldl


####### Files

bin_PROGRAMS = kassistant.bin kassistantcom

kassistant_bin_SOURCES = main.cpp browser.cpp helpview.cpp docserver.cpp \
                         cgi.cpp noteditor.cpp helpcom.cc helpcom_impl.cpp  \
                         mypushbutton.cpp assistant.cpp

kassistantcom_SOURCES  = kassistantcom.cpp helpcom.cc

METASOURCES            = assistant.moc browser.moc cgi.moc docserver.moc helpview.moc mypushbutton.moc noteditor.moc

DISTCLEANFILES         = $(METASOURCES)

noinst_HEADERS         = browser.h docserver.h cgi.h noteditor.h assistant.h \
                         mypushbutton.h version.h config-kassistant.h

idl_DATA    =  helpcom.idl
idl_HEADERS =  helpcom.h

SUBDIRS     =  answer doc pics anim

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(idldir)

helpcom.h: helpcom.idl
	rm -rf helpcom.h
	$(IDL) $(IDL_FLAGS) helpcom.idl

helpcom.cc: helpcom.idl
	rm -rf helpcom.cc
	$(IDL) $(IDL_FLAGS) helpcom.idl

install-exec-local:
	$(INSTALL_SCRIPT) kassistant $(bindir)/kassistant

#install-data-local: 

uninstall-local:
	-rm $(bindir)/kassistant
#	-rm -f $(APPSDIR)/helpcenter.kdelnk

