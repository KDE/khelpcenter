#ifndef KUPGRADE_H
#define KUPGRADE_H

#include <qobject.h>

class QString;
class QStringList;

/**
 * This class will convert keys from existing config files into the
 * proper equivalents in new config files.  This is useful, for
 * instance, when you rename keys or change the data format or even
 * move the key to a new file.
 *
 * This takes the name of it's own config file in the constructor
 * ("kupgraderc" by default).  The format of the config file is like
 * so:
 *
 * <pre>
 * [key]
 * OldConfig=
 * OldGroup=
 * NewConfig=
 * NewGroup=
 * NewKey=
 * Map=
 * </pre>
 *
 * 'key' is the key to be processed.  'OldConfig' and 'OldGroup' are
 * the existing config filename and group (which may or may not be the
 * new ones).  'NewConfig' and 'NewGroup' are the new names.. if they
 * are left blank then the old config filename and group are used.
 * 'NewKey' is used if you want to change the name of the key.
 *
 * 'Map' is a string list of pairs of mappings.  It is used when you
 * change the format or type of the data.  For instance, say you used
 * to have a data type of "On" or "Off" and you want that to now be
 * "true" and "false".  In this case, you would have
 * 'Map=On,true,Off,false'
 *
 * Here is a concrete example that moves the old proxy settings from
 * the kfmrc file to the new kioslaverc file:
 *
 * <pre>
 * [HTTP-Proxy]
 * OldConfig=kfmrc
 * OldGroup=Browser Settings/Proxy
 * NewConfig=kioslave
 * NewGroup=Proxy Settings
 * NewKey=HttpProxy
 * </pre>
 *
 * This works pretty much automatically.  It does emit a signal
 * @ref signalKey whenever it makes a change, though
 *
 * @author Kurt Granroth <granroth@kde.org>
 */
class KUpgrade : public QObject
{
  Q_OBJECT
public:
  KUpgrade(const QString& config_file = "kupgraderc");
  virtual ~KUpgrade();

  void start();
  void stop();

signals:
  void signalKeyCount(int count);
  void signalKey(const QString& old_key, const QString& old_file,
                 const QString& old_group, const QString& old_data,
                 const QString& new_key, const QString& new_file,
                 const QString& new_group, const QString& new_data);
  void signalCompleted();

protected slots:
  void processNextKey();

protected:
  QString applyMapping(const QString& data, const QStringList& map);

private:
  class KUpgradePrivate;
  KUpgradePrivate *d;
};
#endif // KUPGRADE_H
