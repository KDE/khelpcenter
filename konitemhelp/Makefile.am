
INCLUDES =  $(all_includes)
LDFLAGS = $(all_libraries)
LDADD = -lkfm -lkdecore -lqt -lXext -lX11 -lstdc++ $(LIBSOCKET) $(LIBCOMPAT)

########## Files ##########

bin_PROGRAMS	     = konitemhelp

konitemhelp_SOURCES  = help.cpp

METASOURCES = help.moc

DISTCLEANFILES = $(METASOURCES)

MOCS = help.moc

noinst_HEADERS = help.h 
