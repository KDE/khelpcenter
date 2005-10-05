/**
 * This file is part of KHelpCenter
 *
 * Copyright (c) 2000 Matthias H�lzer-Kl�pfel <hoelzer@kde.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KHC_HTMLSEARCHCONFIG_H
#define KHC_HTMLSEARCHCONFIG_H

#include <qwidget.h>

class KURLRequester;
class KConfig;

namespace KHC {

class HtmlSearchConfig : public QWidget
{
    Q_OBJECT
  public:
    HtmlSearchConfig(QWidget *parent = 0L, const char *name = 0L);
    virtual ~HtmlSearchConfig();

    void load( KConfig * );
    void save( KConfig * );
    void defaults();
    void makeReadOnly();

  signals:
    void changed();

  protected slots:
    void urlClicked(const QString&);

  private:
    KURLRequester *mHtsearchUrl;
    KURLRequester *mIndexerBin;
    KURLRequester *mDbDir;
};

}

#endif
// vim:ts=2:sw=2:et
