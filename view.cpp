/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "view.h"

#include "grantleeformatter.h"
#include "history.h"
#include "khc_debug.h"
#include "mainwindow.h"

#include <dom/html_document.h>
#include <dom/html_head.h>
#include <dom/html_misc.h>

#include <KActionCollection>
#include <KHTMLSettings>
#include <KHTMLView>
#include <KToolBarPopupAction>
#include <KSharedConfig>
#include <docbookxslt.h>

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QScrollBar>
#include <QStandardPaths>
#include <QTextStream>
#include <QWhatsThis>

using namespace KHC;

View::View( QWidget *parentWidget, QObject *parent, KHTMLPart::GUIProfile prof, KActionCollection *col )
    : KHTMLPart( parentWidget, parent, prof ), mState( Docu ), mActionCollection(col)
{
    setJScriptEnabled(false);
    setJavaEnabled(false);
    setPluginsEnabled(false);

    mGrantleeFormatter = new GrantleeFormatter;

    m_fontScaleStepping = 10;

    connect(this, &View::setWindowCaption, this, &View::setTitle);
    connect( this, QOverload<const QString &, const QPoint &>::of(&View::popupMenu), this, &View::showMenu );

    const QString css = langLookup(QStringLiteral("kdoctools5-common/kde-default.css"));
    if (!css.isEmpty())
    {
       QFile css_file(css);
       if (css_file.open(QIODevice::ReadOnly))
       {
          QTextStream s(&css_file);
          QString stylesheet = s.readAll();
          preloadStyleSheet(QStringLiteral("help:/kdoctools5-common/kde-default.css"), stylesheet);
       }
    }

    view()->installEventFilter( this );
}

View::~View()
{
  delete mGrantleeFormatter;
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

    // retrieve the local search path
    const QStringList localDoc = KDocTools::documentationDirs();

    QStringList langs = KLocalizedString::languages();
    langs.append(QStringLiteral("en"));
    langs.removeAll(QStringLiteral("C"));

    // this is kind of compat hack as we install our docs in en/ but the
    // default language is en_US
    for (QStringList::Iterator it = langs.begin(); it != langs.end(); ++it) {
        if (*it == QLatin1String("en_US")) {
            *it = QStringLiteral("en");
        }
    }

    // look up the different languages
    int ldCount = localDoc.count();
    for (int id = 0; id < ldCount; id++) {
        QStringList::ConstIterator lang;
        for (lang = langs.constBegin(); lang != langs.constEnd(); ++lang) {
            search.append(QStringLiteral("%1/%2/%3").arg(localDoc[id], *lang, fname));
        }
    }

    // try to locate the file
    for (QStringList::ConstIterator it = search.constBegin(); it != search.constEnd(); ++it) {

        QFileInfo info(*it);
        if (info.exists() && info.isFile() && info.isReadable()) {
            return *it;
        }

            QString file = (*it).left((*it).lastIndexOf(QLatin1Char('/'))) + QStringLiteral("/index.docbook");
            info.setFile(file);
            if (info.exists() && info.isFile() && info.isReadable()) {
                return *it;
            }
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

  begin( QUrl( QStringLiteral( "khelpcenter:search/result" ) ) );
  mSearchResult = QString();
}

void View::writeSearchResult( const QString &str )
{
  write( str );
  mSearchResult += str;
}

void View::endSearchResult()
{
  end();
  if ( !mSearchResult.isEmpty() )
      Q_EMIT searchResultCacheAvailable();
}

void View::beginInternal( const QUrl &url )
{
  mInternalUrl = url;
  begin( mInternalUrl );
}

QUrl View::internalUrl() const
{
  return mInternalUrl;
}

void View::lastSearch()
{
  if ( mSearchResult.isEmpty() ) return;

  mState = Search;

  begin( QUrl( QStringLiteral( "khelpcenter:search/last" ) ) );
  write( mSearchResult );
  end();
}

void View::slotIncFontSizes()
{
  setFontScaleFactor( fontScaleFactor() + m_fontScaleStepping );
  reloadPage();
}

void View::slotDecFontSizes()
{
  setFontScaleFactor( fontScaleFactor() - m_fontScaleStepping );
  reloadPage();
}

void View::showMenu( const QString& url, const QPoint& pos)
{
  QMenu pop(view());

  if (url.isEmpty())
  {
    QAction *action;
    action = mActionCollection->action(QStringLiteral("go_home"));
    if (action) pop.addAction( action );

    pop.addSeparator();

    action = mActionCollection->action(QStringLiteral("prevPage"));
    if (action) pop.addAction( action );
    action = mActionCollection->action(QStringLiteral("nextPage"));
    if (action) pop.addAction( action);

    pop.addSeparator();

    pop.addAction( History::self().m_backAction );
    pop.addAction( History::self().m_forwardAction );
  }
  else
  {
    QAction *action = pop.addAction(i18n("Copy Link Address"));
    connect(action, &QAction::triggered, this, &View::slotCopyLink);

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
  for (unsigned i = 0; i <= links.length(); ++i) {
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
    Q_EMIT browserExtension()->openUrlRequest(prevURL);
  return true;
}

bool View::nextPage(bool checkOnly)
{
  const DOM::NodeList links = document().getElementsByTagName("link");

  QUrl nextURL = urlFromLinkNode( findLink(links, "next") );

  if (!nextURL.isValid())
    return false;

  if (!checkOnly)
    Q_EMIT browserExtension()->openUrlRequest(nextURL);
  return true;
}

bool View::eventFilter( QObject *o, QEvent *e )
{
  if ( htmlDocument().links().length() == 0 )
    return KHTMLPart::eventFilter( o, e );

  switch ( e->type() ) {
    case QEvent::KeyPress: {
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
      break;
    }
    case QEvent::WhatsThis: {
      QHelpEvent *he = static_cast<QHelpEvent *>( e );
      const QString text = i18n( "<p>Read the topic documentation in this window.<br /><br />Press <b>Space</b>/<b>Shift+Space</b> to scroll, <b>%1</b> to find something, <b>Tab</b>/<b>Shift+Tab</b> to jump, and <b>Enter</b> to follow.</p>",
                                 actionCollection()->action( QStringLiteral("find") )->shortcut().toString( QKeySequence::NativeText ) );
      QWhatsThis::showText( he->globalPos(), text, qobject_cast<QWidget *>( o ) );
      e->accept();
      return true;
    }
    case QEvent::QueryWhatsThis:
      e->accept();
      return true;
    default:
      break;
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

  const QUrl domHrefUrl( domHref.string() );
  if ( !domHrefUrl.isRelative() )
    return domHrefUrl;

  return QUrl(baseURL().toString() +QLatin1Char('/')+ domHref.string());
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

void View::reloadPage()
{
  MainWindow *mainWindow = dynamic_cast<MainWindow *>( qobject_cast<QApplication*>(qApp)->activeWindow() );
  mainWindow->viewUrl(baseURL().url());
}

// vim:ts=2:sw=2:et
