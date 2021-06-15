/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
    qCWarning(KHC_LOG) << "GrantleeFormatter rendering error:" << t->errorString();
  }

  return result;
}

GrantleeFormatter::GrantleeFormatter()
  : d( new Private )
{
  QSharedPointer< Grantlee::FileSystemTemplateLoader > loader( new Grantlee::FileSystemTemplateLoader );
  loader->setTemplateDirs( QStandardPaths::locateAll( QStandardPaths::AppDataLocation, QStringLiteral("templates"), QStandardPaths::LocateDirectory ) );
  d->engine.addTemplateLoader( loader );
}

GrantleeFormatter::~GrantleeFormatter()
{
  delete d;
}

QString GrantleeFormatter::formatOverview( const QString& title, const QString& name, const QString& content )
{
  Grantlee::Template t = d->engine.loadByName( QStringLiteral("index.html") );

  Grantlee::Context ctx;
  ctx.insert( QStringLiteral("title"), title );
  ctx.insert( QStringLiteral("name"), name );
  ctx.insert( QStringLiteral("content"), content );

  return d->format( t, &ctx );
}

QString GrantleeFormatter::formatGlossaryEntry( const GlossaryEntry& entry )
{
  Grantlee::Template t = d->engine.loadByName( QStringLiteral("glossary.html") );

  const GlossaryEntryXRef::List entrySeeAlso = entry.seeAlso();
  QStringList seeAlso;
  seeAlso.reserve( entrySeeAlso.count() );
  for ( const GlossaryEntryXRef &xref : entrySeeAlso )
  {
    seeAlso += QStringLiteral( "<a href=\"glossentry:%1\">%2</a>" ).arg( xref.id(), xref.term() );
  }

  Grantlee::Context ctx;
  ctx.insert( QStringLiteral("htmltitle"), i18nc( "%1 is a glossary term", "KDE Glossary: %1", entry.term() ) );
  ctx.insert( QStringLiteral("title"), i18n( "KDE Glossary" ) );
  ctx.insert( QStringLiteral("term"), entry.term() );
  ctx.insert( QStringLiteral("definition"), entry.definition() );
  ctx.insert( QStringLiteral("seeAlsoCount"), seeAlso.count() );
  ctx.insert( QStringLiteral("seeAlso"), i18n( "See also: %1", seeAlso.join(QLatin1String(", ") ) ) );

  return d->format( t, &ctx );
}

QString GrantleeFormatter::formatSearchResults( const QString& words, const QList<QPair<DocEntry *, QString> >& results )
{
  Grantlee::Template t = d->engine.loadByName( QStringLiteral("search.html") );

  typedef QPair<DocEntry *, QString> Iter;
  QVariantList list;
  list.reserve( results.count() );
  for ( const Iter& it : results )
  {
    QVariantHash h;
    h.insert( QStringLiteral("title"), it.first->name() );
    h.insert( QStringLiteral("content"), it.second );
    list += h;
  }

  Grantlee::Context ctx;
  ctx.insert( QStringLiteral("htmltitle"), i18n( "Search Results" ) );
  ctx.insert( QStringLiteral("title"), i18n( "Search Results for '%1':", words.toHtmlEscaped() ) );
  ctx.insert( QStringLiteral("results"), list );

  return d->format( t, &ctx );
}
