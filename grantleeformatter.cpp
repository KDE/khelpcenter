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
#include "glossary.h"
#include "docentry.h"

#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/template.h>
#include <grantlee/templateloader.h>

#include <KLocalizedString>

#include <QStandardPaths>

using namespace KHC;

class PlainOutputStream : public Grantlee::OutputStream
{
  public:
    PlainOutputStream( QTextStream *stream )
      : Grantlee::OutputStream( stream )
    {
    }

    QString escape( const QString& input ) const override
    {
      return input;
    }

    QSharedPointer<OutputStream> clone( QTextStream *stream ) const override
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

QString GrantleeFormatter::formatGlossaryEntry( const GlossaryEntry& entry )
{
  Grantlee::Template t = d->engine.loadByName( "glossary.html" );

  const GlossaryEntryXRef::List entrySeeAlso = entry.seeAlso();
  QStringList seeAlso;
  seeAlso.reserve( entrySeeAlso.count() );
  foreach ( const GlossaryEntryXRef &xref, entrySeeAlso )
  {
    seeAlso += QString::fromLatin1( "<a href=\"glossentry:%1\">%2</a>" ).arg( xref.id(), xref.term() );
  }

  Grantlee::Context ctx;
  ctx.insert( "htmltitle", i18nc( "%1 is a glossary term", "KDE Glossary: %1", entry.term() ) );
  ctx.insert( "title", i18n( "KDE Glossary" ) );
  ctx.insert( "term", entry.term() );
  ctx.insert( "definition", entry.definition() );
  ctx.insert( "seeAlsoCount", seeAlso.count() );
  ctx.insert( "seeAlso", i18n( "See also: %1", seeAlso.join( ", " ) ) );

  return d->format( t, &ctx );
}

QString GrantleeFormatter::formatSearchResults( const QString& words, const QList<QPair<DocEntry *, QString> >& results )
{
  Grantlee::Template t = d->engine.loadByName( "search.html" );

  typedef QPair<DocEntry *, QString> Iter;
  QVariantList list;
  list.reserve( results.count() );
  foreach ( const Iter& it, results )
  {
    QVariantHash h;
    h.insert( "title", it.first->name() );
    h.insert( "content", it.second );
    list += h;
  }

  Grantlee::Context ctx;
  ctx.insert( "htmltitle", i18n( "Search Results" ) );
  ctx.insert( "title", i18n( "Search Results for '%1':", words.toHtmlEscaped() ) );
  ctx.insert( "results", list );

  return d->format( t, &ctx );
}
