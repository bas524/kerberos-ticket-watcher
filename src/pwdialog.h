#ifndef PWDIALOG_H
#define PWDIALOG_H

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include "krb5_tw_gettext.h"
#include "ui_pwdialog.h"

class PWDialog : public QDialog, private Ui::PWDialog {
  Q_OBJECT

  QIcon _icEye;
  QIcon _icClosedEye;

 public:
  explicit PWDialog(QWidget* parent = nullptr, const char* name = nullptr, bool modal = false, Qt::WindowFlags fl = Qt::WindowType::Widget);

  ~PWDialog();

  void krb5promptSetText(const QString& text);

  void promptEditSetEchoMode(QLineEdit::EchoMode mode);

  QString promptEditText() const;

  void errorLabelSetText(const QString& text);
 private slots:
  void on_btnShowPwd_clicked();
};

#endif  // PWDIALOG_H
