#include <qtextstream.h>
#include <qfile.h>
#include <qfileinfo.h>

#include <kstandarddirs.h>
#include <klocale.h>
#include <kdebug.h>

#include "mainwindow.h"

#include "view.h"
#include "view.moc"

using namespace KHC;

View::View( QWidget *parentWidget, const char *widgetName,
                  QObject *parent, const char *name, KHTMLPart::GUIProfile prof )
    : KHTMLPart( parentWidget, widgetName, parent, name, prof ), mState( Docu )
{
    connect( this, SIGNAL( setWindowCaption( const QString & ) ),
             this, SLOT( setTitle( const QString & ) ) );
             
    QString css = langLookup("common/kde-default.css");
    if (!css.isEmpty())
    {
       QFile css_file(css);
       if (css_file.open(IO_ReadOnly))
       {
          QTextStream s(&css_file);
          QString stylesheet = s.read();
          preloadStyleSheet("help:/common/kde-default.css", stylesheet);
       }
    }
}

bool View::openURL( const KURL &url )
{
    if ( url.protocol().lower() == "about" )
    {
        showAboutPage();
        return true;
    }
    mState = Docu;
    return KHTMLPart::openURL( url );
}

void View::saveState( QDataStream &stream )
{
    stream << mState << mGlossEntry;
    if ( mState == Docu )
        KHTMLPart::saveState( stream );
}

void View::restoreState( QDataStream &stream )
{
    stream >> mState >> mGlossEntry;
    if ( mState == Docu )
        KHTMLPart::restoreState( stream );
    else if ( mState == About )
        showAboutPage();
    else if ( mState == GlossEntry )
        showGlossaryEntry( mGlossEntry );
}

void View::showGlossaryEntry( const GlossaryEntry &entry )
{
    if(mGlossEntry.term()==entry.term())
        return;

    QFile htmlFile( locate("data", "khelpcenter/glossary.html.in" ) );
    if (!htmlFile.open(IO_ReadOnly))
        return;

    emit started( 0 );

    mState = GlossEntry;
    mGlossEntry = entry;

    QString seeAlso;
    if (!entry.seeAlso().isEmpty()) {
        seeAlso = i18n("See also: ");
        GlossaryEntryXRef::List seeAlsos = entry.seeAlso();
        GlossaryEntryXRef::List::ConstIterator it = seeAlsos.begin();
        GlossaryEntryXRef::List::ConstIterator end = seeAlsos.end();
        for (; it != end; ++it) {
            seeAlso += QString::fromLatin1("<a href=\"glossentry:");
            seeAlso += (*it).id();
            seeAlso += QString::fromLatin1("\">") + (*it).term();
            seeAlso += QString::fromLatin1("</a>, ");
        }
        seeAlso = seeAlso.left(seeAlso.length() - 2);
    }

    QTextStream htmlStream(&htmlFile);
    QString htmlSrc = htmlStream.read()
                      .arg( i18n( "KDE Glossary" ) )
                      .arg( entry.term() )
                      .arg( langLookup( "khelpcenter/konq.css" ) )
                      .arg( langLookup( "khelpcenter/pointers.png" ) )
                      .arg( langLookup( "khelpcenter/khelpcenter.png" ) )
                      .arg( langLookup( "khelpcenter/lines.png" ) )
                      .arg(entry.term())
                      .arg(entry.definition())
                      .arg(seeAlso)
                      .arg( langLookup( "khelpcenter/kdelogo2.png" ) );

    begin("help:/khelpcenter/glossary" );
    write(htmlSrc);
    end();
    emit completed();
}

void View::showAboutPage()
{
    QString file = locate( "data", "khelpcenter/intro.html.in" );
    if ( file.isEmpty() )
        return;

    QFile f( file );

    if ( !f.open( IO_ReadOnly ) )
    return;

    mState = About;

    emit started( 0 );

    QTextStream t( &f );

    QString res = t.read();

    res = res.arg( i18n("Conquer your Desktop!") )
          .arg( langLookup( "khelpcenter/konq.css" ) )
          .arg( langLookup( "khelpcenter/pointers.png" ) )
          .arg( langLookup( "khelpcenter/khelpcenter.png" ) )
          .arg( i18n("Help Center") )
          .arg( langLookup( "khelpcenter/lines.png" ) )
          .arg( i18n( "Welcome to the K Desktop Environment" ) )
          .arg( i18n( "The KDE team welcomes you to user-friendly UNIX computing" ) )
          .arg( i18n( "KDE is a powerful graphical desktop environment for Unix workstations. A\n"
                      "KDE desktop combines ease of use, contemporary functionality and outstanding\n"
                      "graphical design with the technological superiority of the Unix operating\n"
                      "system." ) )
          .arg( i18n( "What is the K Desktop Environment?" ) )
          .arg( i18n( "Contacting the KDE Project" ) )
          .arg( i18n( "Supporting the KDE Project" ) )
          .arg( i18n( "Useful links" ) )
          .arg( i18n( "Getting the most out of KDE" ) )
          .arg( i18n( "General Documentation" ) )
          .arg( i18n( "A Quick Start Guide to the Desktop" ) )
          .arg( i18n( "KDE User's guide" ) )
          .arg( i18n( "Frequently asked questions" ) )
          .arg( i18n( "Basic Applications" ) )
          .arg( i18n( "The Kicker Desktop Panel" ) )
          .arg( i18n( "The KDE Control Center" ) )
          .arg( i18n( "The Konqueror File manager and Web Browser" ) )
          .arg( langLookup( "khelpcenter/kdelogo2.png" ) );
    begin( "about:khelpcenter" );
    write( res );
    end();
    emit completed();
}

QString View::langLookup( const QString &fname )
{
    QStringList search;

    // assemble the local search paths
    const QStringList localDoc = KGlobal::dirs()->resourceDirs("html");

    // look up the different languages
    for (int id=localDoc.count()-1; id >= 0; --id)
    {
        QStringList langs = KGlobal::locale()->languageList();
        langs.append( "en" );
        langs.remove( "C" );
        QStringList::ConstIterator lang;
        for (lang = langs.begin(); lang != langs.end(); ++lang)
            search.append(QString("%1%2/%3").arg(localDoc[id]).arg(*lang).arg(fname));
    }

    // try to locate the file
    QStringList::Iterator it;
    for (it = search.begin(); it != search.end(); ++it)
    {
        QFileInfo info(*it);
        if (info.exists() && info.isFile() && info.isReadable())
            return *it;
        
		QString file = (*it).left((*it).findRev('/')) + "/index.docbook";
		info.setFile(file);
		if (info.exists() && info.isFile() && info.isReadable())
			return *it;
    }

    return QString::null;
}

void View::setTitle( const QString &title )
{
    mTitle = title;
}

void View::beginSearchResult()
{
  mState = Search;

  begin();
  mSearchResult = "";
}

void View::writeSearchResult( const QString &str )
{
  write( str );
  mSearchResult += str;
}

void View::endSearchResult()
{
  end();
  if ( !mSearchResult.isEmpty() ) emit searchResultCacheAvailable();
}

void View::lastSearch()
{
  if ( mSearchResult.isEmpty() ) return;
 
  mState = Search;
  
  begin();
  write( mSearchResult );
  end();
}

// vim:ts=2:sw=2:et
