
#include "view.h"

#include "formatter.h"
#include "history.h"

#include <dom/html_document.h>
#include <dom/html_head.h>
#include <dom/html_misc.h>

#include <KActionCollection>
#include <KHTMLSettings>
#include <KHTMLView>
#include <KToolBarPopupAction>

#include <QFileInfo>
#include <QClipboard>
#include <QTextStream>
#include <QKeyEvent>
#include <qguiapplication.h>
#include <QEvent>
#include <QDir>
#include <QScrollBar>

using namespace KHC;

View::View( QWidget *parentWidget, QObject *parent, KHTMLPart::GUIProfile prof, KActionCollection *col )
    : KHTMLPart( parentWidget, parent, prof ), mState( Docu ), mActionCollection(col)
{
    setJScriptEnabled(false);
    setJavaEnabled(false);
    setPluginsEnabled(false);

    mFormatter = new Formatter;
    if ( !mFormatter->readTemplates() ) {
      qDebug() << "Unable to read Formatter templates.";
    }

    m_fontScaleStepping = 10;

    connect( this, SIGNAL( setWindowCaption( const QString & ) ),
             this, SLOT( setTitle( const QString & ) ) );
    connect( this, SIGNAL( popupMenu( const QString &, const QPoint& ) ),
             this, SLOT( showMenu( const QString &, const QPoint& ) ) );

    QString css = langLookup("kdoctools5-common/kde-default.css");
    if (!css.isEmpty())
    {
       QFile css_file(css);
       if (css_file.open(QIODevice::ReadOnly))
       {
          QTextStream s(&css_file);
          QString stylesheet = s.readAll();
          preloadStyleSheet("help:/kdoctools5-common/kde-default.css", stylesheet);
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
    qobject_cast<QGuiApplication*>(qApp)->clipboard()->setText( selectedText() );
}

bool View::openUrl( const QUrl &url )
{
    mState = Docu;
    return KHTMLPart::openUrl( url );
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
}

QString View::langLookup( const QString &fname )
{
    QStringList search;

    // assemble the local search paths
    const QStringList localDoc = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

    // look up the different languages
    for (int id=localDoc.count()-1; id >= 0; --id)
    {
        QDir d(QStringLiteral("%1/doc/HTML/").arg(localDoc[id]));

        foreach(const QString& entry, d.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            search.append(d.absoluteFilePath(entry+'/'+fname));
        }
    }

    // try to locate the file
    QStringList::Iterator it;
    for (it = search.begin(); it != search.end(); ++it)
    {
        QFileInfo info(*it);
        if (info.exists() && info.isFile() && info.isReadable())
            return *it;

		QString file = it->left(it->lastIndexOf('/')) + "/index.docbook";
		info.setFile(file);
		if (info.exists() && info.isFile() && info.isReadable())
			return *it;
    }

    return QString();
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

void View::beginInternal( const QUrl &url )
{
  mInternalUrl = url;
  begin();
}

QUrl View::internalUrl() const
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
  setFontScaleFactor( fontScaleFactor() + m_fontScaleStepping );
}

void View::slotDecFontSizes()
{
  setFontScaleFactor( fontScaleFactor() - m_fontScaleStepping );
}

void View::showMenu( const QString& url, const QPoint& pos)
{
  QMenu pop(view());

  if (url.isEmpty())
  {
    QAction *action;
    action = mActionCollection->action("go_home");
    if (action) pop.addAction( action );

    pop.addSeparator();

    action = mActionCollection->action("prevPage");
    if (action) pop.addAction( action );
    action = mActionCollection->action("nextPage");
    if (action) pop.addAction( action);

    pop.addSeparator();

    pop.addAction( History::self().m_backAction );
    pop.addAction( History::self().m_forwardAction );
  }
  else
  {
    QAction *action = pop.addAction(i18n("Copy Link Address"));
    connect( action, SIGNAL( triggered() ), this, SLOT( slotCopyLink() ) );

    mCopyURL = completeURL(url).url();
  }

  pop.exec(pos);
}

void View::slotCopyLink()
{
  QGuiApplication::clipboard()->setText(mCopyURL);
}

static DOM::HTMLLinkElement findLink(const DOM::NodeList& links, const char *rel)
{ 
  for (unsigned i = 0; i <= links.length(); i++) {
    DOM::HTMLLinkElement link(links.item(i));
    if (link.isNull())
      continue;

    if (link.rel() == rel)
      return link;
  }
  return DOM::HTMLLinkElement();
}

bool View::prevPage(bool checkOnly)
{
  const DOM::NodeList links = document().getElementsByTagName("link");

  QUrl prevURL = urlFromLinkNode( findLink(links, "prev") );

  if (!prevURL.isValid())
    return false;

  if (!checkOnly)
    emit browserExtension()->openUrlRequest(prevURL);
  return true;
}

bool View::nextPage(bool checkOnly)
{
  const DOM::NodeList links = document().getElementsByTagName("link");

  QUrl nextURL = urlFromLinkNode( findLink(links, "next") );

  if (!nextURL.isValid())
    return false;

  if (!checkOnly)
    emit browserExtension()->openUrlRequest(nextURL);
  return true;
}

bool View::eventFilter( QObject *o, QEvent *e )
{
  if ( e->type() != QEvent::KeyPress ||
       htmlDocument().links().length() == 0 )
    return KHTMLPart::eventFilter( o, e );

  QKeyEvent *ke = static_cast<QKeyEvent *>( e );
  if ( ke->modifiers() & Qt::ShiftModifier && ke->key() == Qt::Key_Space ) {
    // If we're on the first page, it does not make sense to go back.
    if ( baseURL().path().endsWith( QLatin1String("/index.html") ) )
      return KHTMLPart::eventFilter( o, e );

    const QScrollBar * const scrollBar = view()->verticalScrollBar();
    if ( scrollBar->value() == scrollBar->minimum() ) {
      if (prevPage())
         return true;
    }
  } else if ( ke->key() == Qt::Key_Space ) {
    const QScrollBar * const scrollBar = view()->verticalScrollBar();
    if ( scrollBar->value() == scrollBar->maximum() ) {
      if (nextPage())
        return true;
    }
  }
  return KHTMLPart::eventFilter( o, e );
}

QUrl View::urlFromLinkNode( const DOM::HTMLLinkElement &link ) const
{
  if ( link.isNull() )
    return QUrl();

  DOM::DOMString domHref = link.href();
  if (domHref.isNull())
    return QUrl();

  return QUrl(baseURL().toString() +'/'+ domHref.string());
}

void View::slotReload( const QUrl &url )
{
  const_cast<KHTMLSettings *>( settings() )->init( KSharedConfig::openConfig().data() );
  KParts::OpenUrlArguments args = arguments();
  args.setReload( true );
  setArguments( args );
  if ( url.isEmpty() )
    openUrl( baseURL() );
  else
    openUrl( url );
}

#include "view.moc"
// vim:ts=2:sw=2:et
