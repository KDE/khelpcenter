/*
 *  kwelcome.h - part of KWelcome
 *
 *  Copyright (C) 1998,99 Matthias Elter (me@kde.org)
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

#ifndef KWELCOME_H
#define KWELCOME_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <ktmainwindow.h>
#include "kwelcomewidget.h"

class KWelcome : public KTMainWindow
{
  Q_OBJECT
public:
  KWelcome();
  
private:
  KWelcomeWidget *view;
};

#endif // KWELCOME_H 
