#include <kupgradewidget.h>
#include <kupgrade.h>

#include <stdlib.h>

#include <kiconloader.h>
#include <kicontheme.h>
#include <klocale.h>
#include <klineedit.h>

#include <qdir.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qprogressbar.h>
#include <qpushbutton.h>

KUpgradeWizard::KUpgradeWizard()
  : KWizard(),
    select(new KDEDIRSelector(this)),
    convert(new KUpgradeWidget(this))
{
  addPage(select, i18n("Directory Selection"));
  setHelpEnabled(select, false);

  addPage(convert, i18n("KConfig Conversion"));
  setHelpEnabled(convert, false);
  setFinishEnabled(convert, true);
}

KUpgradeWizard::~KUpgradeWizard()
{
}

void KUpgradeWizard::showPage( QWidget *page )
{
  if ( page == convert )
  {
    QString KDEDIRS(select->kde1LocalPath());
    QString last;
    if ( ::getenv("KDEDIRS") )
      last = ::getenv("KDEDIRS");
    else if ( ::getenv("KDEDIR") )
      last = ::getenv("KDEDIR");

    if ( !last.isNull() )
      KDEDIRS += ":" + last;

    ::setenv("KDEDIRS", KDEDIRS.latin1(), true);
    ::setenv("KDEHOME", select->kde2LocalPath().latin1(), true);
    KGlobal::_instance = new KInstance(KGlobal::instance()->aboutData());
  }

  KWizard::showPage( page );
}

//////////////////////////////////////////////////////////////////////

KDEDIRSelector::KDEDIRSelector(QWidget *parent)
  : QWidget(parent)
{
  layoutGUI();
}

KDEDIRSelector::~KDEDIRSelector()
{
}

QString KDEDIRSelector::kde1LocalPath() const
{
  return kde1_edit->text();
}

QString KDEDIRSelector::kde2LocalPath() const
{
  return kde2_edit->text();
}

void KDEDIRSelector::layoutGUI()
{
  QLabel *kde1_label = new QLabel(i18n("KDE 1.x Local Directory"), this);
  kde1_edit = new KLineEdit(this);
  QString kde1_path(QDir::homeDirPath() + "/.kde/");
  kde1_edit->setText(kde1_path);

  QLabel *kde2_label = new QLabel(i18n("KDE 2.0 Local Directory"), this);
  kde2_edit = new KLineEdit(this);
  QString kde2_path(::getenv("KDEHOME"));
  if (kde2_path.isEmpty())
    kde2_path = QDir::homeDirPath() + "/.kde2/";
  kde2_edit->setText(kde2_path);

  QVBoxLayout *layout = new QVBoxLayout(this, 5);
  layout->addWidget(kde1_label);
  layout->addWidget(kde1_edit);
  layout->addWidget(kde2_label);
  layout->addWidget(kde2_edit);
  layout->addStretch(1);
}

//////////////////////////////////////////////////////////////////////

KUpgradeWidget::KUpgradeWidget(QWidget *parent)
  : QWidget(parent),
    worker(new KUpgrade),
    total(0)
{
  worker = new KUpgrade;
  connect(worker, SIGNAL(signalKey(const QString&, const QString&,
                                   const QString&, const QString&,
                                   const QString&, const QString&,
                                   const QString&, const QString&)),
          this,   SLOT(slotKey(const QString&, const QString&, const QString&,
                               const QString&, const QString&, const QString&,
                               const QString&, const QString&)));
  connect(worker, SIGNAL(signalKeyCount(int)),
          this,   SLOT(slotKeyCount(int)));
  connect(worker, SIGNAL(signalCompleted()),
          this,   SLOT(slotCompleted()));

  layoutGUI();
}

KUpgradeWidget::~KUpgradeWidget()
{
  worker->stop();
  delete worker; worker = 0;
}

void KUpgradeWidget::slotStartStop()
{
  if (start_button->text() == i18n("&Start") )
  {
    worker->start();
    progress->reset();
    start_button->setText( i18n("&Stop") );
  }
  else
  {
    worker->stop();
    start_button->setText( i18n("&Start") );
  }
}

void KUpgradeWidget::slotCompleted()
{
  start_button->setText(i18n("&Done"));
  start_button->setEnabled(false);
}

void KUpgradeWidget::layoutGUI()
{
  old_info = new QListBox(this);
  old_info->setSelectionMode( QListBox::NoSelection );
  old_info->setMinimumSize(200, 200);
  new_info = new QListBox(this);
  new_info->setSelectionMode( QListBox::NoSelection );
  new_info->setMinimumSize(200, 200);
  progress = new QProgressBar(this);
  progress->setCenterIndicator(true);

  start_button = new QPushButton(i18n("&Start"), this);
  connect(start_button, SIGNAL(clicked()),
          this,         SLOT(slotStartStop()));

  // static label stuff
  QLabel *old_label = new QLabel(i18n("Old Data"), this);
  old_label->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  QLabel *new_label = new QLabel(i18n("New Data"), this);
  new_label->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  QLabel *arrow = new QLabel(this);
  arrow->setPixmap(BarIcon("forward", KIcon::SizeSmall));

  QVBoxLayout *old_layout = new QVBoxLayout;
  old_layout->addWidget(old_label);
  old_layout->addWidget(old_info);

  QVBoxLayout *new_layout = new QVBoxLayout;
  new_layout->addWidget(new_label);
  new_layout->addWidget(new_info);

  QHBoxLayout *box_layout = new QHBoxLayout;
  box_layout->addLayout(old_layout);
  box_layout->addWidget(arrow);
  box_layout->addLayout(new_layout);

  QHBoxLayout *progress_layout = new QHBoxLayout;
  progress_layout->addWidget(progress);
  progress_layout->addWidget(start_button);

  QVBoxLayout *layout = new QVBoxLayout(this, 5);
  layout->addLayout(box_layout);
  layout->addLayout(progress_layout);
}

void KUpgradeWidget::slotKey(const QString& old_key, const QString& old_file,
                             const QString& old_group, const QString& old_data,
                             const QString& new_key, const QString& new_file,
                             const QString& new_group, const QString& new_data)
{
  static int count = 1;

  progress->setProgress(count++);

  QString file("---- %1 ----");
  QString group("[%1]");
  QString key("%1=%2");

  old_info->insertItem(file.arg(old_file));
  old_info->insertItem(group.arg(old_group));
  old_info->insertItem(key.arg(old_key).arg(old_data));

  new_info->insertItem(file.arg(new_file));
  new_info->insertItem(group.arg(new_group));
  new_info->insertItem(key.arg(new_key).arg(new_data));
}

void KUpgradeWidget::slotKeyCount(int count)
{
  total = count;
  progress->setTotalSteps(total);
}
