/*
 *  khc_testpart.cc - part of the KDE Help Center
 *
 *  Copyright (c) 1999 Matthias Elter (me@kde.org)
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

#include "khc_testpart.h"

TestPart::TestPart()
{
  cout << "+TestPart" << endl;
  
  // OpenParts need this, since OPPartif
  // is not derived from QWidget.
  setWidget( this );

  // create the HTML widget as a child
  m_pHTML = new KHTMLView( this );
  m_pHTML->show();
}

TestPart::~TestPart()
{
  cout << "-TestPart" << endl;
}
  
void TestPart::resizeEvent( QResizeEvent * )
{
  cerr << "Resizing" << endl;
  // Keep the html widget in shape
  m_pHTML->setGeometry( 0, 0, width(), height() );
}

void TestPart::begin( const char* url )
{
  m_pHTML->begin( url );
}

void TestPart::write( const char* text )
{
  m_pHTML->write( text );
}

void TestPart::end()
{
  m_pHTML->end();
}

void TestPart::parse()
{
  m_pHTML->parse();
}

#include "khc_testpart.moc"
