#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwidget.h>
#include <qregexp.h>
#include <qprogressdialog.h>
#include <qdir.h>


#include <kapp.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <klocale.h>
#include <kconfig.h>


#include "progressdialog.h"
#include "htmlsearch.moc"


HTMLSearch::HTMLSearch()
  : QObject(), _proc(0)
{
}


QString HTMLSearch::dataPath(QString _lang)
{
  return kapp->dirs()->saveLocation("data", QString("khelpcenter/%1").arg(_lang));
}


void HTMLSearch::scanDir(QString dir)
{
  QDir d(dir, "*.html", QDir::Name|QDir::IgnoreCase, QDir::Files | QDir::Readable);
  QStringList const &list = d.entryList();
  QStringList::ConstIterator it;
  for (it=list.begin(); it != list.end(); ++it)
    {
      _files.append(dir + "/" + *it);
      progress->setFilesScanned(++_filesScanned);
    }

  QDir d2(dir, QString::null, QDir::Name|QDir::IgnoreCase, QDir::Dirs);
  QStringList const &dlist = d2.entryList();
  for (it=dlist.begin(); it != dlist.end(); ++it)
    if (*it != "." && *it != "..")
      {
	scanDir(dir + "/" + *it);
	kapp->processEvents();
      }
}


bool HTMLSearch::saveFilesList(QString _lang)
{
  QStringList dirs;

  // throw away old files list
  _files.clear();

  // open config file
  KConfig *config = new KConfig("khelpcenterrc");
  config->setGroup("Scope");

  // add KDE help dirs
  if (config->readBoolEntry("KDE"))
    dirs = kapp->dirs()->findDirs("html", _lang);

  // TODO: Man and Info!!

  // add local urls
  QStringList add = config->readListEntry("Paths");
  QStringList::Iterator it;
  for (it = add.begin(); it != add.end(); ++it)
    dirs.append(*it);

  _filesScanned = 0;

  for (it = dirs.begin(); it != dirs.end(); ++it)
    scanDir(*it);

  return true;
}


bool HTMLSearch::createConfig(QString _lang)
{
  QString fname = dataPath(_lang) + "/htdig.conf";
  
  // locate the common dir
  QString wrapper = locate("data", QString("khelpcenter/%1/wrapper.html").arg(_lang));
  if (wrapper.isEmpty())
    wrapper = locate("data", QString("khelpcenter/en/wrapper.html"));
  if (wrapper.isEmpty())
    return false;
  wrapper = wrapper.left(wrapper.length() - 12);

  // locate the image dir
  QString images = locate("data", "khelpcenter/pics/star.png");
  if (images.isEmpty())
    return false;
  images = images.left(images.length() - 8);

  QFile f(fname);
  if (f.open(IO_WriteOnly))
    {
      kdDebug() << "Writing config for " << _lang << " to " << fname << endl;
      
      QTextStream ts(&f);

      ts << "database_dir:\t\t" << dataPath(_lang) << endl;
      ts << "start_url:\t\t`" << dataPath(_lang) << "/files`" << endl;
      ts << "local_urls:\t\thttp://localhost/=/" << endl;
      ts << "local_urls_only:\ttrue" << endl;
      ts << "maximum_pages:\t\t1" << endl;
      ts << "image_url_prefix:\t\t" << images << endl;
      ts << "star_image:\t\t" << images << "star.png" << endl;
      ts << "star_blank:\t\t" << images << "star_blank.png" << endl;
      ts << "compression_level:\t\t6" << endl;
      ts << "max_hop_count:\t\t0" << endl;

      ts << "search_results_wrapper:\t" << wrapper << "wrapper.html" << endl;
      ts << "nothing_found_file:\t" << wrapper << "nomatch.html" << endl;
      ts << "syntax_error_file:\t" << wrapper << "syntax.html" << endl;
      ts << "bad_word_list:\t\t" << wrapper << "bad_words" << endl;

      f.close();
      return true;
    }

  return false;
}


#define CHUNK_SIZE 100


bool HTMLSearch::generateIndex(QString _lang, QWidget *parent)
{
  if (_lang == "C")
    _lang = "en";

  if (!createConfig(_lang))
    return false;

  // create progress dialog
  progress = new ProgressDialog(parent);
  progress->show();
  kapp->processEvents();

  // create files list ----------------------------------------------
  if (!saveFilesList(_lang))
    return false;

  progress->setState(1);

  // run htdig ------------------------------------------------------
  KConfig *config = new KConfig("khelpcenterrc", true);
  KConfigGroupSaver saver(config, "htdig");
  QString exe = config->readEntry("htdig", kapp->dirs()->findExe("htdig"));
  if (exe.isEmpty())
    return false;

  bool initial = true;
  bool done = false;
  int  count = 0;

  _filesToDig = _files.count();
  progress->setFilesToDig(_filesToDig);
  _filesDigged = 0;

  QDir d; d.mkdir(dataPath(_lang));

  while (!done)
    {
      // kill old process
      delete _proc;

      // prepare new process
      _proc = new KProcess();
      *_proc << exe << "-c" << dataPath(_lang)+"/htdig.conf";
      if (initial)
	{
	  *_proc << "-i";
	  initial = false;
	}

      kdDebug() << "Running htdig" << endl;

      //      connect(_proc, SIGNAL(receivedStdout(KProcess *,char*,int)),
      //	      this, SLOT(htdigStdout(KProcess *,char*,int)));
      connect(_proc, SIGNAL(processExited(KProcess *)),
	      this, SLOT(htdigExited(KProcess *)));

      _htdigRunning = true;
      
      // write out file
      QFile f(dataPath(_lang)+"/files");
      if (f.open(IO_WriteOnly))
	{
	  QTextStream ts(&f);

	  for (int i=0; i<CHUNK_SIZE; ++i, ++count)
	    if (count < _filesToDig)
	      ts << "http://localhost/" + _files[count] << endl;
	    else
	      {
		done = true;
		break;
	      }
	}
      else
	{
	  kdDebug() << "Could not open `files` for writing" << endl;
	  return false;
	}

      // execute htdig
      _proc->start(KProcess::NotifyOnExit, KProcess::Stdout);      
      while (_htdigRunning && _proc->isRunning())
	kapp->processEvents();
      
      if (!_proc->normalExit() || _proc->exitStatus() != 0)
	{
	  delete _proc;
	  delete progress;
	  return false;
	}

      _filesDigged += CHUNK_SIZE;
      progress->setFilesDigged(_filesDigged);
      kapp->processEvents();
    }

  progress->setState(2);

  // run htmerge -----------------------------------------------------
  exe = config->readEntry("htmerge", kapp->dirs()->findExe("htmerge"));
  if (exe.isEmpty())
    return false;

  delete _proc;
  _proc = new KProcess();
  *_proc << exe << "-c" << dataPath(_lang)+"/htdig.conf";

  kdDebug() << "Running htmerge" << endl;

  connect(_proc, SIGNAL(processExited(KProcess *)),
	  this, SLOT(htmergeExited(KProcess *)));

  _htmergeRunning = true;

  _proc->start(KProcess::NotifyOnExit, KProcess::Stdout);

  while (_htmergeRunning && _proc->isRunning())
    kapp->processEvents();
  
  if (!_proc->normalExit() || _proc->exitStatus() != 0)
    {
      delete _proc;
      delete progress;
      return false;
    }

  delete _proc;

  progress->setState(3);
  kapp->processEvents();

  delete progress;

  return true;
}



void HTMLSearch::htdigStdout(KProcess *, char *buffer, int len)
{
  QString line = QString(buffer).left(len);
  
  int cnt=0, index=-1;
  while ( (index = line.find("http://", index+1)) > 0)
    cnt++;
  _filesDigged += cnt;

  cnt=0, index=-1;
  while ( (index = line.find("not changed", index+1)) > 0)
    cnt++;
  _filesDigged -= cnt;

  progress->setFilesDigged(_filesDigged);
}


void HTMLSearch::htdigExited(KProcess *)
{
  kdDebug() << "htdig terminated" << endl;
  _htdigRunning = false;
}


void HTMLSearch::htmergeExited(KProcess *)
{
  kdDebug() << "htmerge terminated" << endl;
  _htmergeRunning = false;
}


void HTMLSearch::htsearchStdout(KProcess *, char *buffer, int len)
{
  _searchResult += QString(buffer).left(len);
}


void HTMLSearch::htsearchExited(KProcess *)
{
  kdDebug() << "htsearch terminated" << endl;
  _htsearchRunning = false;
}


QString HTMLSearch::search(QString _lang, QString words, QString method, int matches,
			   QString format, QString sort)
{
  if (_lang == "C")
    _lang = "en";

  createConfig(_lang);

  QString result = dataPath(_lang)+"/result.html";

  // run htsearch ----------------------------------------------------
  KConfig *config = new KConfig("khelpcenterrc", true);
  KConfigGroupSaver saver(config, "htdig");
  QString exe = config->readEntry("htsearch", kapp->dirs()->findExe("htsearch"));
  if (exe.isEmpty())
    return QString::null;

  _proc = new KProcess();
  *_proc << exe << "-c" << dataPath(_lang)+"/htdig.conf" <<
    QString("words=%1;method=%2;matchesperpage=%3;format=%4;sort=%5").arg(words).arg(method).arg(matches).arg(format).arg(sort);

  kdDebug() << "Running htsearch" << endl;

  connect(_proc, SIGNAL(receivedStdout(KProcess *,char*,int)),
	  this, SLOT(htsearchStdout(KProcess *,char*,int)));
  connect(_proc, SIGNAL(processExited(KProcess *)),
	  this, SLOT(htsearchExited(KProcess *)));

  _htsearchRunning = true;
  _searchResult = "";

  _proc->start(KProcess::NotifyOnExit, KProcess::Stdout);

  while (_htsearchRunning && _proc->isRunning())
    kapp->processEvents();
  
  if (!_proc->normalExit() || _proc->exitStatus() != 0)
    { 
      kdDebug() << "Error running htsearch... returning now" << endl;
      delete _proc;
      return QString::null;
    }

  delete _proc;

  // modify the search result
  _searchResult = _searchResult.replace(QRegExp("http://localhost/"), "file:/");
  _searchResult = _searchResult.replace(QRegExp("Content-type: text/html"), "");

  // dump the search result
  QFile f(result);
  if (f.open(IO_WriteOnly))
    {
      QTextStream ts(&f);

      ts << _searchResult << endl;

      f.close();
      return result;
    }

  return QString::null;
}
