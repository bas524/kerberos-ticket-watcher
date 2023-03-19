#ifndef KINITDIALOG_H
#define KINITDIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include "krb5_tw_gettext.h"
#include "ui_kinitdialog.h"

class KinitDialog : public QDialog, private Ui::KinitDialog {
  Q_OBJECT

  QIcon _icEye;
  QIcon _icClosedEye;

 public:
  explicit KinitDialog(QWidget* parent = nullptr, const char* name = nullptr, bool modal = false, Qt::WindowFlags fl = Qt::WindowType::Widget);
  ~KinitDialog() override;

  void errorLabelSetText(const QString& text);

  void userLineEditSetText(const QString& text);

  QString userLineEditText() const;

  void realmLineEditSetText(const QString& text);

  QString realmLineEditText() const;

  void passwordLineEditSetFocus();

  QString passwordLineEditText();

  void forwardCheckBoxSetChecked(bool check);

  bool forwardCheckBoxIsChecked() const;

  void proxyCheckBoxSetChecked(bool check);

  bool proxyCheckBoxIsChecked() const;

  void lifetimeSpinBoxSetValue(int v);

  int lifetimeSpinBoxValue() const { return lifetimeSpinBox->value(); }

  void lifetimeUnitComboBoxSetCurrentText(const QString& text);

  QString lifetimeUnitComboBoxCurrentText() const;

  void renewtimeSpinBoxSetValue(int v);

  int renewtimeSpinBoxValue() const;

  void renewUnitComboBoxSetCurrentText(const QString& text);

  QString renewUnitComboBoxCurrentText() const;

  void renewCheckBoxSetChecked(bool check);

  bool renewCheckBoxIsChecked() const;

  bool ldapServerIsEmpty() const;

  QString ldapServerText() const;

  void ldapServerSetText(const QString& text);

 public slots:
  void changeDetailsState(int state);
  void changeRenewState(int state);
 private slots:
  void on_btnShowPwd_clicked();
};

#endif
