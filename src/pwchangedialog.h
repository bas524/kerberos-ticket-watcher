#ifndef PWCHANGEDIALOG_H
#define PWCHANGEDIALOG_H

#include <QDialog>
#include <QString>

#include "krb5_tw_gettext.h"

#include "ui_pwchangedialog.h"

class PWChangeDialog : public QDialog, private Ui::PWChangeDialog {
  Q_OBJECT

  QIcon _icEye;
  QIcon _icClosedEye;

 public:
  explicit PWChangeDialog(QWidget* parent = nullptr, const char* name = nullptr, bool modal = false, Qt::WindowFlags fl = Qt::WindowType::Widget);

  ~PWChangeDialog();

  void titleTextLabelSetText(const QString& text);

  void errorLabelSetText(const QString& text);

  QString pwEdit1Text() const;

  QString pwEdit2Text() const;
 private slots:
  void on_btnShowPwd_clicked();
  void on_pwEdit1_textChanged(const QString& arg1);
  void on_pwEdit2_textChanged(const QString& arg1);
};

#endif  // PWCHANGEDIALOG_H
