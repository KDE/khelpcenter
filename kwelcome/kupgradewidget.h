#ifndef KUPGRADEWIDGET_H
#define KUPGRADEWIDGET_H
#include <qwidget.h>
#include <kwizard.h>

class KUpgradeWidget;
class KDEDIRSelector;

class KUpgrade;
class KLineEdit;
class QListBox;
class QProgressBar;
class QPushButton;

class KUpgradeWizard : public KWizard
{
  Q_OBJECT
public:
  KUpgradeWizard();
  virtual ~KUpgradeWizard();

  void showPage( QWidget * );

private:
  KDEDIRSelector *select;
  KUpgradeWidget *convert;
};

class KDEDIRSelector : public QWidget
{
  Q_OBJECT
public:
  KDEDIRSelector(QWidget *parent = 0);
  virtual ~KDEDIRSelector();

  QString kde1LocalPath() const;
  QString kde2LocalPath() const;

protected:
  void layoutGUI();

private:
  KLineEdit *kde1_edit;
  KLineEdit *kde2_edit;
};

class KUpgradeWidget : public QWidget
{
  Q_OBJECT
public:
  KUpgradeWidget(QWidget *parent = 0);
  virtual ~KUpgradeWidget();

protected slots:
  void slotStartStop();
  void slotCompleted();

  void slotKeyCount(int count);
  void slotKey(const QString& old_key, const QString& old_file,
               const QString& old_group, const QString& old_data,
               const QString& new_key, const QString& new_file,
               const QString& new_group, const QString& new_data);

protected:
  void layoutGUI();

private:
  KUpgrade *worker;

  QListBox *old_info;
  QListBox *new_info;
  QProgressBar *progress;
  QPushButton *start_button;

  int total;
};
#endif // KUPGRADEWIDGET_H
