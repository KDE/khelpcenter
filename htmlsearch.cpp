#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwidget.h>
#include <qregexp.h>
#include <qprogressdialog.h>


#include <kapp.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <klocale.h>


#include "htmlsearch.moc"


HTMLSearch::HTMLSearch()
  : QObject()
{
}


QString HTMLSearch::dataPath(QString _lang)
{
  return kapp->dirs()->saveLocation("data", QString("khelpcenter/%1").arg(_lang));
}


bool HTMLSearch::saveFilesList(QString _lang)
{
  _files = kapp->dirs()->findAllResources("html", QString("%1/*.html").arg(_lang), true);
  
  QString fname = dataPath(_lang) + "/files";
  QFile f(fname);
  if (f.open(IO_WriteOnly))
    {
      kdDebug() << "Saving files list for " << _lang << " into " << fname << endl;

      QTextStream ts(&f);

      QStringList::Iterator it;
      for (it = _files.begin(); it != _files.end(); ++it)
	ts << "http://localhost" << *it << endl;

      f.close();

      return true;
    }

  return false;
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

  QFile f(fname);
  if (f.open(IO_WriteOnly))
    {
      kdDebug() << "Writing config for " << _lang << " to " << fname << endl;
      
      QTextStream ts(&f);

      ts << "database_dir:\t\t" << dataPath(_lang) << endl;
      ts << "start_url:\t\t`" << dataPath(_lang) << "/files`" << endl;
      ts << "local_urls:\t\thttp://localhost/=/" << endl;
      ts << "local_urls_only:\ttrue" << endl;
      ts << "local_default_doc:\t\tindex.html" << endl;
      ts << "maximum_pages:\t\t1" << endl;

      ts << "search_results_wrapper:\t" << wrapper << "wrapper.html" << endl;
      ts << "nothing_found_file:\t" << wrapper << "nomatch.html" << endl;
      ts << "syntax_error_file:\t" << wrapper << "syntax.html" << endl;
      ts << "bad_word_list:\t\t" << wrapper << "bad_words" << endl;

      f.close();
      return true;
    }

  return false;
}


bool HTMLSearch::generateIndex(QString _lang, bool init, QWidget *parent)
{
  if (!saveFilesList(_lang))
    return false;
  if (!createConfig(_lang))
    return false;

  // create progress dialog
  progress = new QProgressDialog(parent, 0, true);
  progress->setCaption(i18n("Regenerating index"));
  progress->setLabelText(i18n("KHelpCenter is regenerating the index\n"
			      "used to perform efficient searches.\n"
			      "This may take a while..."));
  progress->setMinimumDuration(0);
  progress->setCancelButtonText(QString::null);

  // run htdig ------------------------------------------------------

  QString exe = kapp->dirs()->findExe("htdig");
  if (exe.isEmpty())
    return false;

  _proc = new KProcess();
  *_proc << exe << "-v" << "-c" << dataPath(_lang)+"/htdig.conf";
  if (init)
    *_proc << "-i";

  kdDebug() << "Running htdig" << endl;

  connect(_proc, SIGNAL(receivedStdout(KProcess *,char*,int)),
	  this, SLOT(htdigStdout(KProcess *,char*,int)));
  connect(_proc, SIGNAL(processExited(KProcess *)),
	  this, SLOT(htdigExited(KProcess *)));

  _htdigRunning = true;
  _filesToDig = _files.count();
  _filesDigged = 0;

  progress->setTotalSteps(_filesToDig);

  _proc->start(KProcess::NotifyOnExit, KProcess::Stdout);

  while (_htdigRunning)
    kapp->processEvents();
  
  if (!_proc->normalExit() || _proc->exitStatus() != 0)
    {
      delete _proc;
      delete progress;
      return false;
    }

  delete _proc;


  // run htmerge -----------------------------------------------------

  exe = kapp->dirs()->findExe("htmerge");
  if (exe.isEmpty())
    return false;

  _proc = new KProcess();
  *_proc << exe << "-c" << dataPath(_lang)+"/htdig.conf";

  kdDebug() << "Running htmerge" << endl;

  connect(_proc, SIGNAL(processExited(KProcess *)),
	  this, SLOT(htmergeExited(KProcess *)));

  _htmergeRunning = true;

  _proc->start(KProcess::NotifyOnExit, KProcess::Stdout);

  while (_htmergeRunning)
    kapp->processEvents();
  
  if (!_proc->normalExit() || _proc->exitStatus() != 0)
    {
      delete _proc;
      delete progress;
      return false;
    }

  delete _proc;
  delete progress;

  return true;
}



void HTMLSearch::htdigStdout(KProcess *proc, char *buffer, int len)
{
  QString line = QString(buffer).left(len);
  
  int cnt=0, index=-1;
  while ( (index = line.find("http://", index+1)) > 0)
    cnt++;

  _filesDigged += cnt;

  progress->setProgress(_filesDigged);

  kdDebug() << "processed " << _filesDigged << " of " << _filesToDig << " files" << endl;
}


void HTMLSearch::htdigExited(KProcess *proc)
{
  kdDebug() << "htdig terminated" << endl;
  _htdigRunning = false;
}


void HTMLSearch::htmergeExited(KProcess *proc)
{
  kdDebug() << "htmerge terminated" << endl;
  _htmergeRunning = false;
}


void HTMLSearch::htsearchStdout(KProcess *proc, char *buffer, int len)
{
  _searchResult += QString(buffer).left(len);
}


void HTMLSearch::htsearchExited(KProcess *proc)
{
  kdDebug() << "htsearch terminated" << endl;
  _htsearchRunning = false;
}


QString HTMLSearch::search(QString _lang, QString words, QString method, int matches,
			   QString format, QString sort)
{
  createConfig(_lang);

  QString result = dataPath(_lang)+"/result.html";

  // run htsearch ----------------------------------------------------

  QString exe = kapp->dirs()->findExe("htsearch");
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

  while (_htsearchRunning)
    kapp->processEvents();
  
  if (!_proc->normalExit() || _proc->exitStatus() != 0)
    { 
      delete _proc;
      return QString::null;
    }

  delete _proc;

  // modify the search result
  _searchResult = _searchResult.replace(QRegExp("http://localhost/"), "file://");
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
