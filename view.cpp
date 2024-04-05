/*
    SPDX-FileCopyrightText: 2002 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "view.h"

#include "grantleeformatter.h"
#include "khc_debug.h"

#include <KActionCollection>
#include <KSharedConfig>
#include <KLocalizedString>
#include <docbookxslt.h>
#include <KIO/TransferJob>

#include <QApplication>
#include <QClipboard>
#include <QWebEngineScript>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineScriptCollection>
#include <QWebEngineUrlRequestJob>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMenu>
#include <QStandardPaths>
#include <QTextStream>
#include <QWhatsThis>
#include <QBuffer>
#include <QFileDialog>
#include <QContextMenuEvent>
#include <QWebEngineUrlScheme>
#include <QWebEngineDownloadRequest>

using namespace KHC;
class HelpUrlSchemeHandler : public QWebEngineUrlSchemeHandler {
    void requestStarted(QWebEngineUrlRequestJob* job) override {
        qCDebug(KHC_LOG) << job->requestUrl();
        KIO::TransferJob* kiojob = KIO::get(job->requestUrl(), KIO::NoReload);
        QByteArray result;
        connect(kiojob, &KIO::TransferJob::data, this, [&result](KJob*, QByteArray data) {
            result += data;
        });
        kiojob->exec();
        auto buffer = new QBuffer(job);
        buffer->setData(result);
        job->reply("text/html", buffer);
    }
};

View::View( QWidget *parentWidget, KActionCollection *col )
    : QWebEngineView( parentWidget), mState( Docu ), mActionCollection(col)
{
    {
        QWebEngineUrlScheme customScheme("help");
        customScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored | QWebEngineUrlScheme::LocalScheme
                              | QWebEngineUrlScheme::LocalAccessAllowed);
        customScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
        QWebEngineUrlScheme::registerScheme(customScheme);
    }
    {
        QWebEngineUrlScheme customScheme("khelpcenter");
        customScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored | QWebEngineUrlScheme::LocalScheme
                              | QWebEngineUrlScheme::LocalAccessAllowed);
        customScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
        QWebEngineUrlScheme::registerScheme(customScheme);
    }
    {
        QWebEngineUrlScheme customScheme("man");
        customScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored | QWebEngineUrlScheme::LocalScheme
                              | QWebEngineUrlScheme::LocalAccessAllowed);
        customScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
        QWebEngineUrlScheme::registerScheme(customScheme);
    }
    {
        QWebEngineUrlScheme customScheme("info");
        customScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored | QWebEngineUrlScheme::LocalScheme
                              | QWebEngineUrlScheme::LocalAccessAllowed);
        customScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
        QWebEngineUrlScheme::registerScheme(customScheme);
    }
    {
        QWebEngineUrlScheme customScheme("glossentry");
        customScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored | QWebEngineUrlScheme::LocalScheme
                              | QWebEngineUrlScheme::LocalAccessAllowed);
        customScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
        QWebEngineUrlScheme::registerScheme(customScheme);
    }

    page()->settings()->setUnknownUrlSchemePolicy(QWebEngineSettings::AllowAllUnknownUrlSchemes);
    page()->settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled,true);

    mGrantleeFormatter = new GrantleeFormatter;
    page()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);

    page()->profile()->installUrlSchemeHandler(QByteArrayLiteral("help"),new HelpUrlSchemeHandler);
    page()->profile()->installUrlSchemeHandler(QByteArrayLiteral("khelpcenter"),new HelpUrlSchemeHandler);
    page()->profile()->installUrlSchemeHandler(QByteArrayLiteral("man"),new HelpUrlSchemeHandler);
    page()->profile()->installUrlSchemeHandler(QByteArrayLiteral("info"),new HelpUrlSchemeHandler);
    QString css = langLookup(QStringLiteral("kdoctools6-common/kde-default.css"));
    if (!css.isEmpty())
    {

        QFile css_file(css);
        if (css_file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&css_file);
            QString stylesheet = stream.readAll().replace(QLatin1Char('"'),QLatin1Char(' '));
            QWebEngineScript script;
            QString s = QString::fromLatin1("(function() {"\
                                            "    css = document.createElement('style');"\
                                            "    css.type = 'text/css';"\
                                            "    css.id = '%1';"\
                                            "    document.head.appendChild(css);"\
                                            "    css.innerText = \"%2\";"\
                                            "})()").arg(QStringLiteral("default")).arg(stylesheet.simplified());
            script.setName(QStringLiteral("default"));
            script.setSourceCode(s);
            script.setInjectionPoint(QWebEngineScript::DocumentReady);
            script.setRunsOnSubFrames(true);
            script.setWorldId(QWebEngineScript::ApplicationWorld);
            page()->scripts().insert(script);

        }
    }
    page()->installEventFilter( this );
    connect(page()->profile(), &QWebEngineProfile::downloadRequested, this, &View::downloadRequested);
}

View::~View()
{
  delete mGrantleeFormatter;
}

void View::copySelectedText()
{
    qobject_cast<QGuiApplication*>(qApp)->clipboard()->setText( selectedText() );
}

QString View::langLookup( const QString &fname )
{
    qDebug() << " View::langLookup" << fname;
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

void View::beginSearchResult()
{
  mState = Search;
  mSearchResult = QString();
}

void View::writeSearchResult( const QString &str )
{
  mSearchResult += str;
}

void View::endSearchResult()
{
  setInternalHtml(mSearchResult, QUrl( QStringLiteral( "khelpcenter:search/result" ) ) );
  if ( !mSearchResult.isEmpty() )
    Q_EMIT searchResultCacheAvailable();
}

void View::setInternalHtml(const QString& data,  const QUrl &url )
{
  mInternalUrl = url;
  setHtml(data, url);
}

QUrl View::internalUrl() const
{
  return mInternalUrl;
}

void View::lastSearch()
{
  if ( mSearchResult.isEmpty() ) return;

  mState = Search;
  setHtml( mSearchResult, QUrl( QStringLiteral( "khelpcenter:search/last" ) )  );
}
void View::slotReload( const QUrl &url )
{
  if ( url.isEmpty() )
    reload();
  else
    load( url );
}

void View::contextMenuEvent(QContextMenuEvent *ev)
{
    QMenu menu;
    QAction *act = pageAction(QWebEnginePage::Back);
    if (act->isEnabled()) {
        menu.addAction(act);
    }
    act = pageAction(QWebEnginePage::Forward);
    if (act->isEnabled()) {
        menu.addAction(act);
    }

    if (!menu.actions().isEmpty()) {
        auto separator = new QAction(&menu);
        separator->setSeparator(true);
        menu.addAction(separator);
    }

    act = pageAction(QWebEnginePage::Copy);
    if (act->isEnabled() && hasSelection()) {
        act->setIcon(QIcon::fromTheme(QStringLiteral("edit-copy")));
        menu.addAction(act);
    }
    if (!menu.actions().isEmpty()) {
        auto separator = new QAction(&menu);
        separator->setSeparator(true);
        menu.addAction(separator);
    }
    act = pageAction(QWebEnginePage::Reload);
    if (act->isEnabled()) {
        menu.addAction(act);
    }
    act = pageAction(QWebEnginePage::CopyLinkToClipboard);
    if (act->isEnabled()) {
        menu.addAction(act);
    }
    act = pageAction(QWebEnginePage::SavePage);
    if (act->isEnabled()) {
        auto separator = new QAction(&menu);
        separator->setSeparator(true);
        menu.addAction(separator);
        act->setIcon(QIcon::fromTheme(QStringLiteral("document-save")));
        menu.addAction(act);
    }
    menu.exec(ev->globalPos());
}

void View::downloadRequested(QWebEngineDownloadRequest *download)
{
    const QString filename = QFileDialog::getSaveFileName(this, i18n("Save Page"));
    if (!filename.isEmpty()) {
        download->setSavePageFormat(QWebEngineDownloadRequest::SingleHtmlSaveFormat);
        download->setDownloadDirectory(QFileInfo(filename).path());
        download->setDownloadFileName(QFileInfo(filename).fileName());
        download->accept();
    } else {
        download->cancel();
    }
}

#include "moc_view.cpp"

// vim:ts=2:sw=2:et

