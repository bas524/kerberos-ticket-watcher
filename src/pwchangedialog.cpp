#include "pwchangedialog.h"

PWChangeDialog::PWChangeDialog(QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl), _icEye(":/eye.svg"), _icClosedEye(":/eye-close.svg") {
  setModal(modal);
  setAccessibleName(name);
  setupUi(this);

  QSize size32(32, 32);
  QList<QSize> qlistSizes = _icClosedEye.availableSizes();
  QList<QSize>::const_iterator it =
      std::lower_bound(qlistSizes.begin(), qlistSizes.end(), size32, [](const QSize &a, const QSize &b) { return a.width() < b.width(); });
  QSize result = (it != qlistSizes.end()) ? *it : size32;

  btnShowPwd->setFixedSize(result);
  btnShowPwd->setIcon(_icClosedEye);
  btnShowPwd->setIconSize(result);
  btnShowPwd->setToolTip(ki18n("Show password"));
  pwEdit1->setEchoMode(QLineEdit::Password);
  pwEdit2->setEchoMode(QLineEdit::Password);
}

PWChangeDialog::~PWChangeDialog() {}

void PWChangeDialog::titleTextLabelSetText(const QString &text) { titleTextLabel->setText(text); }

void PWChangeDialog::errorLabelSetText(const QString &text) { errorLabel->setText(text); }

QString PWChangeDialog::pwEdit1Text() const { return pwEdit1->text(); }

QString PWChangeDialog::pwEdit2Text() const { return pwEdit2->text(); }

void PWChangeDialog::on_btnShowPwd_clicked() {
  switch (pwEdit1->echoMode()) {
    case QLineEdit::Password:
      btnShowPwd->setIcon(_icEye);
      pwEdit1->setEchoMode(QLineEdit::Normal);
      pwEdit2->setEchoMode(QLineEdit::Normal);
      btnShowPwd->setToolTip(ki18n("Hide password"));
      break;
    default:
      btnShowPwd->setIcon(_icClosedEye);
      pwEdit1->setEchoMode(QLineEdit::Password);
      pwEdit2->setEchoMode(QLineEdit::Password);
      btnShowPwd->setToolTip(ki18n("Show password"));
      break;
  }
}

void PWChangeDialog::on_pwEdit1_textChanged(const QString &arg1) {
  if (pwEdit1->text() != pwEdit2->text()) {
    pwEdit1->setStyleSheet("border: 1px solid red");
    pwEdit2->setStyleSheet("border: 1px solid red");
  } else {
    pwEdit1->setStyleSheet("border: 1px solid green");
    pwEdit2->setStyleSheet("border: 1px solid green");
  }
}

void PWChangeDialog::on_pwEdit2_textChanged(const QString &arg1) {
  if (pwEdit1->text() != pwEdit2->text()) {
    pwEdit1->setStyleSheet("border: 1px solid red");
    pwEdit2->setStyleSheet("border: 1px solid red");
  } else {
    pwEdit1->setStyleSheet("border: 1px solid green");
    pwEdit2->setStyleSheet("border: 1px solid green");
  }
}
