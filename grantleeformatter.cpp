/*
  This file is part of the KDE Help Center

  Copyright (c) 2016 Pino Toscano <pino@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA  02110-1301, USA
*/

#include "grantleeformatter.h"

#include "khc_debug.h"

#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/template.h>
#include <grantlee/templateloader.h>

#include <QStandardPaths>

using namespace KHC;

class PlainOutputStream : public Grantlee::OutputStream
{
  public:
    PlainOutputStream( QTextStream *stream )
      : Grantlee::OutputStream( stream )
    {
    }

    virtual QString escape( const QString& input ) const
    {
      return input;
    }

    virtual QSharedPointer<OutputStream> clone( QTextStream *stream ) const
    {
      return QSharedPointer<OutputStream>( new PlainOutputStream( stream ) );
    }
};

struct GrantleeFormatter::Private
{
  QString format( Grantlee::Template t, Grantlee::Context *ctx );

  Grantlee::Engine engine;
};

QString GrantleeFormatter::Private::format( Grantlee::Template t, Grantlee::Context *ctx )
{
  QString result;
  QTextStream textStream( &result );
  PlainOutputStream stream( &textStream );
  t->render( &stream, ctx );
  if ( t->error() )
  {
    khcWarning() << "GrantleeFormatter rendering error:" << t->errorString();
  }

  return result;
}

GrantleeFormatter::GrantleeFormatter()
  : d( new Private )
{
  QSharedPointer< Grantlee::FileSystemTemplateLoader > loader( new Grantlee::FileSystemTemplateLoader );
  loader->setTemplateDirs( QStandardPaths::locateAll( QStandardPaths::DataLocation, "templates", QStandardPaths::LocateDirectory ) );
  d->engine.addTemplateLoader( loader );
}

GrantleeFormatter::~GrantleeFormatter()
{
  delete d;
}

QString GrantleeFormatter::formatOverview( const QString& title, const QString& name, const QString& content )
{
  Grantlee::Template t = d->engine.loadByName( "index.html" );

  Grantlee::Context ctx;
  ctx.insert( "title", title );
  ctx.insert( "name", name );
  ctx.insert( "content", content );

  return d->format( t, &ctx );
}
