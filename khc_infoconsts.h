/*
 *  khc_infoconsts.h - part of the KDE Help Center
 *
 *  Copyright (C) 2001 Wojciech Smigaj (achu@klub.chip.pl)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __KHC_INFOCONSTS__
#define __KHC_INFOCONSTS__

#include <qstring.h>

const uint RETRIEVE_NAME            = 1;    
const uint RETRIEVE_TITLE           = 2;   
const uint RETRIEVE_NEIGHBOURS      = 4;
const uint RETRIEVE_CONTENTS        = 8;

const uint ERR_NONE                 = 0;         
const uint ERR_NO_MORE_NODES        = 1;   
const uint ERR_FILE_UNAVAILABLE     = 2;
const uint ERR_NO_IND_TABLE         = 3;
const uint ERR_NO_HIERARCHY         = 4;

const unsigned int INFODIRS = 9;
const QString INFODIR[] = {"", "/usr/info/", "/usr/share/info/",
			   "/usr/lib/info/",
			   "/usr/local/info/", "/usr/local/lib/info/",
			   "/usr/X11R6/info/", "/usr/X11R6/lib/info/",
			   "/usr/X11R6/lib/xemacs/info/"};



#endif // __KHC_INFOCONSTS__
