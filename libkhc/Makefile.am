#    This file is part of the KDE HelpCenter
#    Copyright (C) 1999  Matthias Elter <me@kde.org>

#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.

#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.

#    You should have received a copy of the GNU Library General Public License
#    along with this library; see the file COPYING.LIB.  If not, write to
#    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.

INCLUDES = $(all_includes) $(MICO_INCLUDES)

IDL_FLAGS = --query-server-for-narrow --relative-paths --any -I$(idldir)
lib_LTLIBRARIES = libkhc.la
libkhc_la_LDFLAGS = $(all_libraries) -version-info 1:0
libkhc_la_LIBADD = -lkparts

libkhc_la_SOURCES = browser.idl khc_baseview.cc khc_history.cc

include_HEADERS = browser.h khc_history.h khc_baseview.h

DISTCLEANFILES = browser.cc browser.h

browser_IDLDIR = $(idldir)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../po/libkhc.pot
