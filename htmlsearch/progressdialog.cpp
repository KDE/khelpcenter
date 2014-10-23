#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>




ProgressDialog::ProgressDialog(QWidget *parent, const char *name)
  : QDialog( parent )
{
  setWindowTitle( i18n("Generating Index") );
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
  QWidget *mainWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  mainLayout->addWidget(mainWidget);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  //PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
  mainLayout->addWidget(buttonBox);
  buttonBox->button(QDialogButtonBox::Cancel)->setDefault(true);
  setObjectName( name );
  setModal( false );

  QGridLayout *grid = new QGridLayout(mainWidget);
  //PORT QT5 grid->setSpacing(spacingHint());

  QLabel *l = new QLabel(i18n("Scanning for files"), mainWidget);
  grid->addWidget(l, 0, 1, 1,2);

  filesScanned = new QLabel(plainPage());
  grid->addWidget(filesScanned, 1,2);
  setFilesScanned(0);

  check1 = new QLabel(plainPage());
  grid->addWidget(check1, 0,0);

  l = new QLabel(i18n("Extracting search terms"), mainWidget);
  grid->addWidget(l, 2, 1, 1,2);

  bar = new QProgressBar(plainPage());
  grid->addWidget(bar, 3,2);

  check2 = new QLabel(plainPage());
  grid->addWidget(check2, 2,0);

  l = new QLabel(i18n("Generating index..."), mainWidget);
  grid->addWidget(l, 4, 1, 1,2);

  check3 = new QLabel(plainPage());
  grid->addWidget(check3, 4,0);

  setState(0);

  setMinimumWidth(300);
}


void ProgressDialog::setFilesScanned(int n)
{
  filesScanned->setText(i18n("Files processed: %1", n));
}


void ProgressDialog::setFilesToDig(int n)
{
  bar->setRange(0, n);
}


void ProgressDialog::setFilesDigged(int n)
{
  bar->setValue(n);
}


void ProgressDialog::setState(int n)
{
  QPixmap unchecked = QPixmap(locate("data", "khelpcenter/pics/unchecked.xpm"));
  QPixmap checked = QPixmap(locate("data", "khelpcenter/pics/checked.xpm"));

  check1->setPixmap( n > 0 ? checked : unchecked);  
  check2->setPixmap( n > 1 ? checked : unchecked);  
  check3->setPixmap( n > 2 ? checked : unchecked);  
}
