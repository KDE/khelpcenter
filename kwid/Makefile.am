INCLUDES = $(all_includes)
LDFLAGS  = $(all_libraries) $(KDE_RPATH)
LDADD    = $(LIB_KDECORE) 

####### Files

bin_PROGRAMS	        = identifyWidget highlightWidget

identifyWidget_SOURCES	= widentify.cpp eyes.cpp kwid.cpp

highlightWidget_SOURCES = highlight.cpp eyes.cpp kwid.cpp

METASOURCES             = eyes.moc kwid.moc

noinst_HEADERS          = eyes.h kwid.h

messages:
