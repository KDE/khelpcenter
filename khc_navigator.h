/*
 *  khc_navigator.h - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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

#ifndef __khc_navigator_h__
#define __khc_navigator_h__


#include <qptrlist.h>
#include <kparts/browserextension.h>
#include <kparts/part.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qdict.h>
#include <klistview.h>

#include <regex.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <map>

class QPushButton;

class KListView;
class KService;
class KProcess;
class KProcIO;

class SearchWidget;
class khcNavigatorItem;
class khcNavigator;
class SearchEngine;
class khcInfoNode;
class khcInfoHierarchyMaker;
class KHCView;

class SectionItem : public QListViewItem
{
  public:
    SectionItem(QListViewItem *, const QString &);

    virtual void setOpen(bool);
};

class TOCListView: public KListView
{
	public:
    TOCListView( QWidget *parent );

    QString application() const { return m_application; }
	void setApplication( const QString &application ) { m_application = application; }

  private:
    QString m_application;
};

class TOCListViewItem : public QListViewItem
{
  public:
    TOCListViewItem( TOCListViewItem *parent, const QString &text );
    TOCListViewItem( TOCListViewItem *parent, QListViewItem *after, const QString &text );
    TOCListViewItem( TOCListView *parent, const QString &text );
    TOCListViewItem( TOCListView *parent, QListViewItem *after, const QString &text );

	virtual QString link() const = 0;

    TOCListView *toc() const;
};

class TOCChapterItem : public TOCListViewItem
{
  public:
    TOCChapterItem( TOCListView *parent, const QString &title, const QString &name );
    TOCChapterItem( TOCListView *parent, QListViewItem *after, const QString &title, const QString &name );

	virtual void setOpen( bool open );
	QString link() const;

  private:
    QString m_name;
};

class TOCSectionItem : public TOCListViewItem
{
  public:
    TOCSectionItem( TOCChapterItem *parent, const QString &title, const QString &name );
	TOCSectionItem( TOCChapterItem *parent, QListViewItem *after, const QString &title, const QString &name );
 
    QString link() const;

  private:
    QString m_name;
};

class khcNavigatorExtension : public KParts::BrowserExtension
{
    Q_OBJECT
  public:
    khcNavigatorExtension(KParts::ReadOnlyPart *part, const char *name=0) :
      KParts::BrowserExtension( part, name ) {}
    virtual ~khcNavigatorExtension() {}

  public slots:
    void slotItemSelected(const QString&);
    void print() {}
};

class khcNavigator : public KParts::ReadOnlyPart
{
    Q_OBJECT
  public:
    khcNavigator( KHCView *, QWidget *parentWidget, QObject *widget,
                  const char *name=0);
    virtual ~khcNavigator();

    virtual bool openURL( const KURL &url );

  protected:
    bool openFile();
    khcNavigatorExtension * m_extension;
};

class khcNavigatorWidget : public QWidget
{
    Q_OBJECT
  public:
    struct GlossaryEntry {
      GlossaryEntry() {}
      GlossaryEntry(const QString &t, const QString &d, const QStringList &sa)
      {
        term = t;
        definition = d;
        seeAlso = sa;
      }

     QString term;
      QString definition;
      QStringList seeAlso;
    };
    
    khcNavigatorWidget(KHCView *, QWidget *parent=0, const char *name=0);
    virtual ~khcNavigatorWidget();

    GlossaryEntry glossEntry(const QString &term) const { return *glossEntries[term]; }

  public slots:
    void slotURLSelected(QString url);
    void slotItemSelected(QListViewItem* index);
    void slotItemExpanded(QListViewItem* index);
    void slotInfoHierarchyCreated(uint key, uint nErrorCode, const khcInfoNode* pRootNode);
    void slotCleanHierarchyMakers();
    void slotGlossaryItemSelected(QListViewItem* item);
    void slotShowPage(QWidget *);

    void slotSearch();

    void slotShowSearchResult( const QString & );

  signals:
    void itemSelected(const QString& itemURL);
    void glossSelected(const khcNavigatorWidget::GlossaryEntry& entry);

  protected slots:
    void slotSearchFinished();
    void slotSearchTextChanged( const QString & );

  private slots:
    void getScrollKeeperContentsList(KProcIO *proc);
    void meinprocExited(KProcess *);
    void slotTOCItemSelected( QListViewItem *item );
    /* Cog-wheel animation handling -- enable after creating the icons
    void slotAnimation();
    */

  private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void setupGlossaryTab();
    void setupTOCTab();
    void resetTOCTree();
    void fillTOCTree( const QDomDocument &doc ); 
    void buildGlossary();
    void buildTree();
    void clearTree();
    QString decodeEntities(const QString &s) const;

    void buildInfoSubTree(khcNavigatorItem *parent);
    QString findInfoDirFile();
    bool readInfoDirFile(QString& sFileContents);
    bool parseInfoSubjectLine(QString sLine, QString& sItemTitle, QString& sItemURL);
    void addChildren(const khcInfoNode* pParentNode, khcNavigatorItem* pParentTreeItem);

    void buildManSubTree(khcNavigatorItem *parent);

    void insertPlugins();
    void insertScrollKeeperItems();
    int insertScrollKeeperSection(khcNavigatorItem *parentItem,QDomNode sectNode);
    void insertScrollKeeperDoc(khcNavigatorItem *parentItem,QDomNode docNode);

    bool appendEntries (const QString &dirName,  khcNavigatorItem *parent, QPtrList<khcNavigatorItem> *appendList);
    bool processDir(const QString &dirName, khcNavigatorItem *parent,  QPtrList<khcNavigatorItem> *appendList);

    void hideSearch();

    QListViewItem *byTopicItem, *alphabItem;
    KListView *contentsTree, *glossaryTree;
    TOCListView *tocTree;

    SearchWidget *mSearchWidget;

    QTabWidget *mTabWidget;

    QFrame *mSearchFrame;
    QLineEdit *mSearchEdit;
    QPushButton *mSearchButton;

    QPtrList<khcNavigatorItem> manualItems, pluginItems, scrollKeeperItems;

    regex_t compInfoRegEx;
    std::map<khcNavigatorItem*, khcInfoHierarchyMaker*> hierarchyMakers;
    QTimer cleaningTimer;

    /* Cog-wheel animation handling -- enable after creating the icons
    struct AnimationInfo
    {
        AnimationInfo( const char * _iconBaseName, uint _iconCount, const QPixmap & _originalPixmap )
            : iconBaseName(_iconBaseName), iconCount(_iconCount), iconNumber(1), originalPixmap(_originalPixmap) {}
        AnimationInfo() : iconCount(0) {}
        QCString iconBaseName;
        uint iconCount;
        uint iconNumber;
        QPixmap originalPixmap;
    };
    typedef QMap<khcNavigatorItem *, AnimationInfo> MapCurrentOpeningSubjects;
    MapCurrentOpeningSubjects m_mapCurrentOpeningSubjects;

    QTimer *m_animationTimer;

    void startAnimation(khcNavigatorItem * item, const char * iconBaseName = "kde", uint iconCount = 6);
    void stopAnimation(khcNavigatorItem * item);
    */

    bool mScrollKeeperShowEmptyDirs;
    QString mScrollKeeperContentsList;
    
    QDict<GlossaryEntry> glossEntries;
    enum { NeedRebuild, CacheOk, ListViewOk} glossaryHtmlStatus;
    QString glossaryHtmlFile;
    QString glossarySource;

    SearchEngine *mSearchEngine;

    KHCView *mView;
};

inline QDataStream &operator<<( QDataStream &stream, const khcNavigatorWidget::GlossaryEntry &e )
{ return stream << e.term << e.definition << e.seeAlso; }

inline QDataStream &operator>>( QDataStream &stream, khcNavigatorWidget::GlossaryEntry &e )
{ return stream >> e.term >> e.definition >> e.seeAlso; }
 
#endif
