/*
 *  khc_testpart.h - part of the KDE Help Center
 *
 *  Copyright (c) 199 Matthias Elter (me@kde.org)
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

#ifndef __khc_testpart_h__
#define __khc_testpart_h__

#include "khelpcenter.h"

#include <khtmlview.h>
#include <qwidget.h>

#include <opPart.h>

class TestPart : virtual public QWidget,
		 virtual public OPPartIf,
		 virtual public KHelpCenter::TestPart_skel
{
  Q_OBJECT
public:
  TestPart();
  virtual ~TestPart();
  
  virtual void begin( const char* url );
  virtual void write( const char* text );
  virtual void end();
  virtual void parse();
  
protected:
  void resizeEvent( QResizeEvent * );
  
  KHTMLView* m_pHTML;
};

#endif
