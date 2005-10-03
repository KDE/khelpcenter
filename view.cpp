#include "view.h"

#include "formatter.h"
#include "history.h"

#include <dom/html_document.h>
#include <dom/html_misc.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kdebug.h>
#include <khtml_settings.h>
#include <khtmlview.h>
#include <klocale.h>
#include <kmenu.h>
#include <kstandarddirs.h>

#include <qfileinfo.h>
#include <qclipboard.h>
//Added by qt3to4:
#include <QTextStream>
#include <QKeyEvent>
#include <QEvent>

using namespace KHC;

View::View( QWidget *parentWidget, const char *widgetName,
                  QObject *parent, const char *name, KHTMLPart::GUIProfile prof, KActionCollection *col )
    : KHTMLPart( parentWidget, widgetName, parent, name, prof ), mState( Docu ), mActionCollection(col)
{
    mFormatter = new Formatter;
    if ( !mFormatter->readTemplates() ) {
      kdDebug() << "Unable to read Formatter templates." << endl;
    }

    m_zoomStepping = 10;

    connect( this, SIGNAL( setWindowCaption( const QString & ) ),
             this, SLOT( setTitle( const QString & ) ) );
    connect( this, SIGNAL( popupMenu( const QString &, const QPoint& ) ),
             this, SLOT( showMenu( const QString &, const QPoint& ) ) );
             
    QString css = langLookup("common/kde-default.css");
    if (!css.isEmpty())
    {
       QFile css_file(css);
       if (css_file.open(QIODevice::ReadOnly))
       {
          QTextStream s(&css_file);
          QString stylesheet = s.read();
          preloadStyleSheet("help:/common/kde-default.css", stylesheet);
       }
    }

    view()->installEventFilter( this );
}

View::~View()
{
  delete mFormatter;
}

void View::copySelectedText()
{
  kapp->clipboard()->setText( selectedText() );
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
    stream << mState;
    if ( mState == Docu )
        KHTMLPart::saveState( stream );
}

void View::restoreState( QDataStream &stream )
{
    stream >> mState;
    if ( mState == Docu )
        KHTMLPart::restoreState( stream );
    else if ( mState == About )
        showAboutPage();
}

void View::showAboutPage()
{
    QString file = locate( "data", "khelpcenter/intro.html.in" );
    if ( file.isEmpty() )
        return;

    QFile f( file );

    if ( !f.open( QIODevice::ReadOnly ) )
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
          .arg( i18n( "KDE is a powerful graphical desktop environment for UNIX workstations. A\n"
                      "KDE desktop combines ease of use, contemporary functionality and outstanding\n"
                      "graphical design with the technological superiority of the UNIX operating\n"
                      "system." ) )
          .arg( i18n( "What is the K Desktop Environment?" ) )
          .arg( i18n( "Contacting the KDE Project" ) )
          .arg( i18n( "Supporting the KDE Project" ) )
          .arg( i18n( "Useful links" ) )
          .arg( i18n( "Getting the most out of KDE" ) )
          .arg( i18n( "General Documentation" ) )
          .arg( i18n( "A Quick Start Guide to the Desktop" ) )
          .arg( i18n( "KDE Users' guide" ) )
          .arg( i18n( "Frequently asked questions" ) )
          .arg( i18n( "Basic Applications" ) )
          .arg( i18n( "The Kicker Desktop Panel" ) )
          .arg( i18n( "The KDE Control Center" ) )
          .arg( i18n( "The Konqueror File manager and Web Browser" ) )
          .arg( langLookup( "khelpcenter/kdelogo2.png" ) );
    begin( KURL( "about:khelpcenter" ) );
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
        
		QString file = (*it).left((*it).lastIndexOf('/')) + "/index.docbook";
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

void View::beginInternal( const KURL &url )
{
  mInternalUrl = url;
  begin();
}

KURL View::internalUrl() const
{
  return mInternalUrl;
}

void View::lastSearch()
{
  if ( mSearchResult.isEmpty() ) return;
 
  mState = Search;
  
  begin();
  write( mSearchResult );
  end();
}

void View::slotIncFontSizes()
{
  setZoomFactor( zoomFactor() + m_zoomStepping );
}

void View::slotDecFontSizes()
{
  setZoomFactor( zoomFactor() - m_zoomStepping );
}

void View::showMenu( const QString& url, const QPoint& pos)
{
  KMenu* pop = new KMenu(view());
  if (url.isEmpty())
  {
    KAction *action;
    action = mActionCollection->action("go_home");
    if (action) action->plug(pop);

    pop->insertSeparator();

    action = mActionCollection->action("prevPage");
    if (action) action->plug(pop);
    action = mActionCollection->action("nextPage");
    if (action) action->plug(pop);

    pop->insertSeparator();

    History::self().m_backAction->plug(pop);
    History::self().m_forwardAction->plug(pop);
  }
  else
  {
    pop->insertItem(i18n("Copy Link Address"), this, SLOT(slotCopyLink()));
    mCopyURL = completeURL(url).url();
  }
	
  pop->exec(pos);
  delete pop;
}

void View::slotCopyLink()
{
  QApplication::clipboard()->setText(mCopyURL);
}

bool View::prevPage(bool checkOnly)
{
  const DOM::HTMLCollection links = htmlDocument().links();

  // The first link on a page (top-left corner) would be the Prev link.
  const DOM::Node prevLinkNode = links.item( 0 );
  KURL prevURL = urlFromLinkNode( prevLinkNode );
  if (!prevURL.isValid())
    return false;

  if (!checkOnly)
    openURL( prevURL );
  return true;
}

bool View::nextPage(bool checkOnly)
{
  const DOM::HTMLCollection links = htmlDocument().links();

  KURL nextURL;

  // If we're on the first page, the "Next" link is the last link
  if ( baseURL().path().endsWith( "/index.html" ) )
    nextURL = urlFromLinkNode( links.item( links.length() - 1 ) );
  else
    nextURL = urlFromLinkNode( links.item( links.length() - 2 ) );

  if (!nextURL.isValid())
    return false;

  // If we get a mail link instead of a http URL, or the next link points
  // to an index.html page (a index.html page is always the first page
  // there can't be a Next link pointing to it!) there's probably nowhere
  // to go. Next link at all.
  if ( nextURL.protocol() == "mailto" ||
       nextURL.path().endsWith( "/index.html" ) )
    return false;

  if (!checkOnly)
    openURL( nextURL );
  return true;
}

bool View::eventFilter( QObject *o, QEvent *e )
{
  if ( e->type() != QEvent::KeyPress ||
       htmlDocument().links().length() == 0 )
    return KHTMLPart::eventFilter( o, e );

  QKeyEvent *ke = static_cast<QKeyEvent *>( e );
  if ( ke->state() & Qt::ShiftModifier && ke->key() == Qt::Key_Space ) {
    // If we're on the first page, it does not make sense to go back.
    if ( baseURL().path().endsWith( "/index.html" ) )
      return KHTMLPart::eventFilter( o, e );

    const QScrollBar * const scrollBar = view()->verticalScrollBar();
    if ( scrollBar->value() == scrollBar->minValue() ) {
      if (prevPage())
         return true;
    }
  } else if ( ke->key() == Qt::Key_Space ) {
    const QScrollBar * const scrollBar = view()->verticalScrollBar();
    if ( scrollBar->value() == scrollBar->maxValue() ) {
      if (nextPage())
        return true;
    }
  }
  return KHTMLPart::eventFilter( o, e );
}

KURL View::urlFromLinkNode( const DOM::Node &n ) const
{
  if ( n.isNull() || n.nodeType() != DOM::Node::ELEMENT_NODE )
    return KURL();

  DOM::Element elem = static_cast<DOM::Element>( n );

  KURL href ( elem.getAttribute( "href" ).string() );
  if ( !href.protocol().isNull() )
    return href;

  QString path = baseURL().path();
  path.truncate( path.lastIndexOf( '/' ) + 1 );
  path += href.url();

  KURL url = baseURL();
  url.setRef( QString::null );
  url.setEncodedPathAndQuery( path );

  return url;
}

void View::slotReload( const KURL &url )
{
  const_cast<KHTMLSettings *>( settings() )->init( kapp->config() );
  KParts::URLArgs args = browserExtension()->urlArgs();
  args.reload = true;
  browserExtension()->setURLArgs( args );
  if ( url.isEmpty() )
    openURL( baseURL() );
  else
    openURL( url );
}

#include "view.moc"
// vim:ts=2:sw=2:et
