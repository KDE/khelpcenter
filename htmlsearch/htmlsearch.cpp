#include <QRegExp>
#include <QDir>
#include <assert.h>

#include <kconfig.h>

#include "progressdialog.h"
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>



HTMLSearch::HTMLSearch()
  : QObject(), _proc(0)
{
}


QString HTMLSearch::dataPath(const QString& _lang)
{
    return kapp->dirs()->saveLocation("data", QString("khelpcenter/%1").arg(_lang));
}


void HTMLSearch::scanDir(const QString& dir)
{
    assert( dir.at( dir.length() - 1 ) == '/' );

    QStringList::ConstIterator it;

    if ( KStandardDirs::exists( dir + "index.docbook" ) ) {
        _files.append(dir + "index.docbook");
        progress->setFilesScanned(++_filesScanned);
    } else {
        QDir d(dir, "*.html", QDir::Name|QDir::IgnoreCase, QDir::Files | QDir::Readable);
        QStringList const &list = d.entryList();
        QString adir = d.canonicalPath () + '/';
        QString file;
        for (it=list.begin(); it != list.end(); ++it)
        {
            file = adir + *it;
            if ( !_files.contains( file ) ) {
                _files.append(file);
                progress->setFilesScanned(++_filesScanned);
            }
        }
    }

    QDir d2(dir, QString(), QDir::Name|QDir::IgnoreCase, QDir::Dirs);
    QStringList const &dlist = d2.entryList();
    for (it=dlist.begin(); it != dlist.end(); ++it)
        if (*it != "." && *it != "..")
        {
            scanDir(dir + *it + '/');
            kapp->processEvents();
        }
}


bool HTMLSearch::saveFilesList(const QString& _lang)
{
    QStringList dirs;

    // throw away old files list
    _files.clear();

    // open config file
    KConfig config("khelpcenterrc");
    KConfigGroup scopeGroup( &config, "Scope" );

    // add KDE help dirs
    if (scopeGroup.readEntry("KDE", true))
        dirs = kapp->dirs()->findDirs("html", _lang + '/');
    //qDebug() << "got " << dirs.count() << " dirs\n";

    // TODO: Man and Info!!

    // add local urls
    QStringList add = scopeGroup.readEntry("Paths", QStringList() );
    QStringList::const_iterator it;
    for (it = add.begin(); it != add.end(); ++it) {
        if ( ( *it ).at( ( *it ).length() - 1 ) != '/' )
            ( *it ) += '/';
        dirs.append(*it);
    }

    _filesScanned = 0;

    for (it = dirs.begin(); it != dirs.end(); ++it)
        scanDir(*it);

    return true;
}


bool HTMLSearch::createConfig(const QString& _lang)
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

    // This is an example replacement for the default bad_words file
    // distributed with ht://Dig. It was compiled by Marjolein Katsma
    // <HSH@taxon.demon.nl>.
    QString bad_words = i18nc( "List of words to exclude from index",
                              "above:about:according:across:actually:\n"
                              "adj:after:afterwards:again:against:all:\n"
                              "almost:alone:along:already:also:although:\n"
                              "always:among:amongst:and:another:any:\n"
                              "anyhow:anyone:anything:anywhere:are:aren:\n"
                              "arent:around:became:because:become:\n"
                              "becomes:becoming:been:before:beforehand:\n"
                              "begin:beginning:behind:being:below:beside:\n"
                              "besides:between:beyond:billion:both:but:\n"
                              "can:cant:cannot:caption:could:couldnt:\n"
                              "did:didnt:does:doesnt:dont:down:during:\n" //krazy:exclude=spelling
                              "each:eight:eighty:either:else:elsewhere:\n"
                              "end:ending:enough:etc:even:ever:every:\n"
                              "everyone:everything:everywhere:except:few:\n"
                              "fifty:first:five:for:former:formerly:forty:\n"
                              "found:four:from:further:had:has:hasnt:have:\n" //krazy:exclude=spelling
                              "havent:hence:her:here:hereafter:hereby:\n"
                              "herein:heres:hereupon:hers:herself:hes:him:\n"
                              "himself:his:how:however:hundred:\n"
                              "inc:indeed:instead:into:isnt:its:\n"
                              "itself:last:later:latter:latterly:least:\n"
                              "less:let:like:likely:ltd:made:make:makes:\n"
                              "many:may:maybe:meantime:meanwhile:might:\n"
                              "million:miss:more:moreover:most:mostly:\n"
                              "mrs:much:must:myself:namely:neither:\n"
                              "never:nevertheless:next:nine:ninety:\n"
                              "nobody:none:nonetheless:noone:nor:not:\n" //krazy:exclude=spelling
                              "nothing:now:nowhere:off:often:once:\n"
                              "one:only:onto:others:otherwise:our:ours:\n"
                              "ourselves:out:over:overall:own:page:per:\n"
                              "perhaps:rather:recent:recently:same:\n"
                              "seem:seemed:seeming:seems:seven:seventy:\n"
                              "several:she:shes:should:shouldnt:since:six:\n" //krazy:exclude=spelling
                              "sixty:some:somehow:someone:something:\n"
                              "sometime:sometimes:somewhere:still:stop:\n"
                              "such:taking:ten:than:that:the:their:them:\n"
                              "themselves:then:thence:there:thereafter:\n"
                              "thereby:therefore:therein:thereupon:these:\n"
                              "they:thirty:this:those:though:thousand:\n"
                              "three:through:throughout:thru:thus:tips:\n"
                              "together:too:toward:towards:trillion:\n"
                              "twenty:two:under:unless:unlike:unlikely:\n"
                              "until:update:updated:updates:upon:\n"
                              "used:using:very:via:want:wanted:wants:\n"
                              "was:wasnt:way:ways:wed:well:were:\n"
                              "werent:what:whats:whatever:when:whence:\n"
                              "whenever:where:whereafter:whereas:whereby:\n"
                              "wherein:whereupon:wherever:wheres:whether:\n"
                              "which:while:whither:who:whoever:whole:\n"
                              "whom:whomever:whose:why:will:with:within:\n"
                              "without:wont:work:worked:works:working:\n"
                              "would:wouldnt:yes:yet:you:youd:youll:your:\n"
                              "youre:yours:yourself:yourselves:youve" );

    QFile f;
    f.setName( dataPath(_lang) + "/bad_words" );
    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream ts( &f );
        QStringList words = bad_words.split( QRegExp ( "[\n:]" ), QString::SkipEmptyParts );
        for ( QStringList::ConstIterator it = words.begin();
              it != words.end(); ++it )
            ts << *it << endl;
        f.close();
    }

    f.setName(fname);
    if (f.open(QIODevice::WriteOnly))
    {
        //qDebug() << "Writing config for " << _lang << " to " << fname;

        QTextStream ts(&f);

        ts << "database_dir:\t\t" << dataPath(_lang) << endl;
        ts << "start_url:\t\t`" << dataPath(_lang) << "/files`" << endl;
        ts << "local_urls:\t\tfile:/=/" << endl;
        ts << "local_urls_only:\ttrue" << endl;
        ts << "maximum_pages:\t\t1" << endl;
        ts << "image_url_prefix:\t" << images << endl;
        ts << "star_image:\t\t" << images << "star.png" << endl;
        ts << "star_blank:\t\t" << images << "star_blank.png" << endl;
        ts << "compression_level:\t6" << endl;
        ts << "max_hop_count:\t\t0" << endl;

        ts << "search_results_wrapper:\t" << wrapper << "wrapper.html" << endl;
        ts << "nothing_found_file:\t" << wrapper << "nomatch.html" << endl;
        ts << "syntax_error_file:\t" << wrapper << "syntax.html" << endl;
        ts << "bad_word_list:\t\t" << dataPath(_lang) << "/bad_words" << endl;
        ts << "external_parsers:\t" << "text/xml\t" << locate( "data", "khelpcenter/meinproc_wrapper" ) << endl;
        f.close();
        return true;
    }

    return false;
}


#define CHUNK_SIZE 15

bool HTMLSearch::generateIndex( const QString & _lang, QWidget *parent)
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
    KConfig config("khelpcenterrc", true);
    KConfigGroup group(&config, "htdig");
    QString exe = group.readPathEntry("htdig", QStandardPaths::findExecutable("htdig"));

    if (exe.isEmpty())
    {
        return false;
    }
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
        _proc = new K3Process();
        *_proc << exe  << "-v" << "-c" << dataPath(_lang)+"/htdig.conf";
        if (initial)
	{
            *_proc << "-i";
            initial = false;
	}

        //qDebug() << "Running htdig";

        connect(_proc, &K3Process::receivedStdout, this, &HTMLSearch::htdigStdout);

        connect(_proc, &K3Process::processExited, this, &HTMLSearch::htdigExited);

        _htdigRunning = true;

        // write out file
        QFile f(dataPath(_lang)+"/files");
        if (f.open(QIODevice::WriteOnly))
	{
            QTextStream ts(&f);

            for (int i=0; i<CHUNK_SIZE; ++i, ++count)
                if (count < _filesToDig) {
                    ts << "file://" + _files[count] << endl;
                } else {
                    done = true;
                    break;
                }
            f.close();
	}
        else
	{
            //qDebug() << "Could not open `files` for writing";
            return false;
	}


        // execute htdig
        _proc->start(K3Process::NotifyOnExit, K3Process::Stdout );

        kapp->enter_loop();

        if (!_proc->normalExit() || _proc->exitStatus() != 0)
	{
            delete _proc;
            delete progress;
            return false;
	}

        // _filesDigged += CHUNK_SIZE;
        progress->setFilesDigged(_filesDigged);
        kapp->processEvents();
    }

    progress->setState(2);

    // run htmerge -----------------------------------------------------
    exe = group.readPathEntry("htmerge", kapp->dirs()->findExe("htmerge"));
    if (exe.isEmpty())
    {
        return false;
    }
    delete _proc;
    _proc = new K3Process();
    *_proc << exe << "-c" << dataPath(_lang)+"/htdig.conf";

    //qDebug() << "Running htmerge";

    connect(_proc, &K3Process::processExited, this, &HTMLSearch::htmergeExited);

    _htmergeRunning = true;

    _proc->start(K3Process::NotifyOnExit, K3Process::Stdout);

    kapp->enter_loop();

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



void HTMLSearch::htdigStdout(K3Process *, char *buffer, int len)
{
    QString line = QString(buffer).left(len);

    int cnt=0, index=-1;
    while ( (index = line.find("file://", index+1)) > 0)
        cnt++;
    _filesDigged += cnt;

    cnt=0;
    index=-1;
    while ( (index = line.find("not changed", index+1)) > 0)
        cnt++;
    _filesDigged -= cnt;

    progress->setFilesDigged(_filesDigged);
}


void HTMLSearch::htdigExited(K3Process *p)
{
    //qDebug() << "htdig terminated " << p->exitStatus();
    _htdigRunning = false;
    kapp->exit_loop();
}


void HTMLSearch::htmergeExited(K3Process *)
{
  //qDebug() << "htmerge terminated";
  _htmergeRunning = false;
  kapp->exit_loop();
}


void HTMLSearch::htsearchStdout(K3Process *, char *buffer, int len)
{
  _searchResult += QString::fromLocal8Bit(buffer,len);
}


void HTMLSearch::htsearchExited(K3Process *)
{
  //qDebug() << "htsearch terminated";
  _htsearchRunning = false;
  kapp->exit_loop();
}


QString HTMLSearch::search( const QString & _lang, const QString & words, const QString & method, int matches,
			   const QString & format, const QString & sort)
{
  if (_lang == "C")
    _lang = "en";

  createConfig(_lang);

  QString result = dataPath(_lang)+"/result.html";

  // run htsearch ----------------------------------------------------
  KConfig *config = new KConfig("khelpcenterrc", true);
  KConfigGroup group(config, "htdig");
  QString exe = group.readPathEntry("htsearch", kapp->dirs()->findExe("htsearch"));
  if (exe.isEmpty())
  {
      delete config;
    return QString();
  }
  _proc = new K3Process();
  *_proc << exe << "-c" << dataPath(_lang)+"/htdig.conf" <<
    QString("words=%1;method=%2;matchesperpage=%3;format=%4;sort=%5").arg(words).arg(method).arg(matches).arg(format).arg(sort);

  //qDebug() << "Running htsearch";

  connect(_proc, &K3Process::receivedStdout, this, &HTMLSearch::htsearchStdout);
  connect(_proc, &K3Process::processExited, this, &HTMLSearch::htsearchExited);

  _htsearchRunning = true;
  _searchResult = "";

  _proc->start(K3Process::NotifyOnExit, K3Process::Stdout);

  kapp->enter_loop();

  if (!_proc->normalExit() || _proc->exitStatus() != 0)
    {
      //qDebug() << "Error running htsearch... returning now";
      delete _proc;
      delete config;
      return QString();
    }

  delete _proc;

  // modify the search result
  _searchResult = _searchResult.replace("http://localhost/", "file:/");
  _searchResult = _searchResult.remove("Content-type: text/html");

  // dump the search result
  QFile f(result);
  if (f.open(QIODevice::WriteOnly))
    {
      QTextStream ts(&f);

      ts << _searchResult << endl;

      f.close();
      delete config;
      return result;
    }
  delete config;
  return QString();
}
