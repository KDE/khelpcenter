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


#include <qlist.h>
#include <kparts/browserextension.h>
#include <kparts/part.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <kprocess.h>

// ACHU
#include <regex.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <map>
//#include "khc_infohierarchymaker.h"
//#include "khc_infonode.h"
// END ACHU

class SearchWidget;
class khcNavigatorItem;
class khcNavigator;
class KListView;
class KService;
class KProcIO;
// ACHU
class khcInfoNode;
class khcInfoHierarchyMaker;
//END ACHU

class SectionItem : public QListViewItem
{
  public:
    SectionItem(QListViewItem *, const QString &);

    virtual void setOpen(bool);
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
};

class khcNavigator : public KParts::ReadOnlyPart
{
    Q_OBJECT

 public:
    khcNavigator(QWidget *parentWidget, QObject *widget, const char *name=0);
    virtual ~khcNavigator();

    virtual bool openURL( const KURL &url );

 protected:
    bool openFile();
    khcNavigatorExtension * m_extension;

};

class khcNavigatorWidget : public QTabWidget
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
    
    khcNavigatorWidget(QWidget *parent=0, const char *name=0);
    virtual ~khcNavigatorWidget();

		GlossaryEntry glossEntry(const QString &term) const { return *glossEntries[term]; }

 public slots:
    void slotURLSelected(QString url);
    void slotItemSelected(QListViewItem* index);
    // ACHU
    void slotItemExpanded(QListViewItem* index);
    void slotInfoHierarchyCreated(uint key, uint nErrorCode, const khcInfoNode* pRootNode);
    void slotCleanHierarchyMakers();
    // END ACHU
    void slotGlossaryItemSelected(QListViewItem* item);
    void slotReloadTree();

 signals:
    void itemSelected(const QString& itemURL);
    void glossSelected(const khcNavigatorWidget::GlossaryEntry& entry);
    void setBussy(bool bussy);

 private slots:
    void getScrollKeeperContentsList(KProcIO *proc);
    void gotMeinprocOutput(KProcess *, char *data, int len);
    void meinprocExited(KProcess *);
    //ACHU
    /* Cog-wheel animation handling -- enable after creating the icons
    void slotAnimation();
    */
    // END ACHU

 private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void setupGlossaryTab();
    void buildGlossaryCache();
    void buildTree();
    void clearTree();
    QString langLookup(const QString &);

    // ACHU
    void buildInfoSubTree(khcNavigatorItem *parent);
    QString findInfoDirFile();
    bool readInfoDirFile(QString& sFileContents);
    bool parseInfoSubjectLine(QString sLine, QString& sItemTitle, QString& sItemURL);
    void addChildren(const khcInfoNode* pParentNode, khcNavigatorItem* pParentTreeItem);
    // END ACHU

    void buildManSubTree(khcNavigatorItem *parent);

    void buildManualSubTree(khcNavigatorItem *parent, QString relPath);
    QString documentationURL(KService *s);

    void insertPlugins();
    void insertScrollKeeperItems();
    int insertScrollKeeperSection(khcNavigatorItem *parentItem,QDomNode sectNode);
    void insertScrollKeeperDoc(khcNavigatorItem *parentItem,QDomNode docNode);

    bool appendEntries (const QString &dirName,  khcNavigatorItem *parent, QList<khcNavigatorItem> *appendList);
    bool processDir(const QString &dirName, khcNavigatorItem *parent,  QList<khcNavigatorItem> *appendList);

    QListViewItem *byTopicItem, *alphabItem;
    KListView *contentsTree, *glossaryTree;
    // SearchWidget *search;

    QList<khcNavigatorItem> staticItems, manualItems, pluginItems, scrollKeeperItems;

    // ACHU
    regex_t compInfoRegEx;
    std::map<khcNavigatorItem*, khcInfoHierarchyMaker*> hierarchyMakers;
    QTimer cleaningTimer;
    // END ACHU

    // ACHU
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
    // END ACHU

    bool mScrollKeeperShowEmptyDirs;
    QString mScrollKeeperContentsList;
    
    QDict<GlossaryEntry> glossEntries;
    KProcess *meinproc;
    QString htmlData;
};

inline QDataStream &operator<<( QDataStream &stream, const khcNavigatorWidget::GlossaryEntry &e )
{ return stream << e.term << e.definition << e.seeAlso; }

inline QDataStream &operator>>( QDataStream &stream, khcNavigatorWidget::GlossaryEntry &e )
{ return stream >> e.term >> e.definition >> e.seeAlso; }
 
#endif
