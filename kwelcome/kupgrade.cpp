#include <kupgrade.h>

#include <kconfig.h>
#include <kglobal.h>

#include <qdir.h>
#include <qmap.h>
#include <qtimer.h>


class KUpgrade::KUpgradePrivate
{
public:
  KUpgradePrivate(const QString& config_file, QObject *parent)
  {
    config = new KConfig( config_file, true, false );
    timer = new QTimer( parent );
  }
  ~KUpgradePrivate()
  {
    delete config; config = 0;
    delete timer; timer = 0;
  }

  KConfig *config;
  QTimer  *timer;
  QStringList groups;
  QStringList::Iterator it;
};

KUpgrade::KUpgrade(const QString& config_file) 
  : d(new KUpgradePrivate( config_file, this ))
{
}

KUpgrade::~KUpgrade()
{
  delete d; d = 0;
}

void KUpgrade::start()
{
  d->groups = d->config->groupList();
  stop();

  emit signalKeyCount( d->groups.count() - 1 );

  connect(d->timer, SIGNAL(timeout()),
          this,     SLOT(processNextKey()));

  d->timer->start(0);
}

void KUpgrade::stop()
{
  d->it = d->groups.begin();
  d->timer->stop();

  disconnect(d->timer);

  emit signalCompleted();
}

void KUpgrade::processNextKey()
{
  if ( d->it == d->groups.end() )
  {
    stop();
    return;
  }

  QString key(*(d->it));

  if ( key == QString::fromLatin1("<default>") )
  {
    d->it++;
    return;
  }
  d->it++;

  static QString keyIsGroup   = QString::fromLatin1("IsGroup");
  static QString keyOldConfig = QString::fromLatin1("OldConfig");
  static QString keyOldGroup  = QString::fromLatin1("OldGroup");
  static QString keyNewConfig = QString::fromLatin1("NewConfig");
  static QString keyNewGroup  = QString::fromLatin1("NewGroup");
  static QString keyNewKey    = QString::fromLatin1("NewKey");
  static QString keyMap       = QString::fromLatin1("Map");

  // keep a pointer just for convenience purposes
  KConfig *cfg = d->config;

  // our key is the group name in the kupgraderc file
  QString old_group( cfg->group() );
  cfg->setGroup( key );

  // this may be inefficient, but lets just read in all possible keys
  // right off the bat
  QString old_config_name(cfg->readEntry(keyOldConfig, QString::null));
  QString old_group_name(cfg->readEntry(keyOldGroup, QString::null));
  QString new_config_name(cfg->readEntry(keyNewConfig, old_config_name));
  QString new_group_name(cfg->readEntry(keyNewGroup, old_group_name));
  QString new_key(cfg->readEntry(keyNewKey, key));
  QStringList map_list(cfg->readListEntry(keyMap));

  // there is a special case of .kderc which isn't handled at all in
  // kde-2
  if ( old_config_name == QString::fromLatin1(".kderc") )
    old_config_name = QDir::homeDirPath() + QString::fromLatin1("/.kderc");

  // first, we open up the old local config file and see if the key
  // exists
  KConfig old_config( old_config_name, true, false );
  if ( !old_config.hasGroup( old_group_name ) )
    return;
  old_config.setGroup( old_group_name );

  // assume that we are only dealing in strings.. the actual type
  // doesn't really matter
  QString old_data( old_config.readEntry( key, QString::null ) );
  if ( old_data.isNull() )
    return;
  
  // apply any mappings if we have 'em
  QString new_data( applyMapping(old_data, map_list) );

  // notify any listening objects on our status
  emit signalKey( key, old_config_name, old_group_name, old_data,
                  new_key, new_config_name, new_group_name, new_data );

  // okay, now we have all of our old info.. let's open up the new
  // config file and write out our info
  KConfig new_config( new_config_name );
  new_config.setGroup( new_group_name );
  new_config.writeEntry( new_key, new_data );
  new_config.sync();

  // restore old settings
  cfg->setGroup( old_group );
}

QString KUpgrade::applyMapping(const QString& data, const QStringList& map)
{
  QString ret( data );
  QStringList::ConstIterator it( map.begin() );
  for ( ; it != map.end(); ++it )
  {
    QString old(*it);
    ++it;
    if ( old == data )
    {
      ret = *it;
      break;
    }
  }
  return ret;
}
