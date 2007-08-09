#ifndef __HTMLSEARCH_H__
#define __HTMLSEARCH_H__





class QWidget;


class K3Process;


class ProgressDialog;


class HTMLSearch : public QObject
{
  Q_OBJECT

public:

  HTMLSearch();

  bool generateIndex(const QString & lang, QWidget *parent=0);

  QString search(const QString & lang, const QString & words, const QString & method="and", int matches=10,
		 const QString & format="builtin-long", const QString & sort="score");


protected Q_SLOTS:

  void htdigStdout(K3Process *proc, char *buffer, int buflen);
  void htdigExited(K3Process *proc);
  void htmergeExited(K3Process *proc);
  void htsearchStdout(K3Process *proc, char *buffer, int buflen);
  void htsearchExited(K3Process *proc);


protected:

  QString dataPath(const QString& lang);

  bool saveFilesList(const QString& lang);
  void scanDir(const QString& dir);

  bool createConfig(const QString& lang);


private:

  QStringList   _files;
  K3Process      *_proc;
  int           _filesToDig, _filesDigged, _filesScanned;
  volatile bool _htdigRunning, _htmergeRunning, _htsearchRunning;
  QString       _searchResult;
  ProgressDialog *progress;

};


#endif
