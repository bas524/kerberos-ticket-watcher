#include "pwdialog.h"

PWDialog::PWDialog(QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl), _icEye(":/eye.svg"), _icClosedEye(":/eye-close.svg") {
  setModal(modal);
  setAccessibleName(name);
  setupUi(this);
  QWidget *wnd = window();
  if (wnd) {
    wnd->setWindowFlags(wnd->windowFlags() | Qt::WindowStaysOnTopHint);
  }
  QSize size32(32, 32);
  QList<QSize> qlistSizes = _icClosedEye.availableSizes();
  QList<QSize>::const_iterator it =
      std::lower_bound(qlistSizes.begin(), qlistSizes.end(), size32, [](const QSize &a, const QSize &b) { return a.width() < b.width(); });
  QSize result = (it != qlistSizes.end()) ? *it : size32;

  btnShowPwd->setFixedSize(result);
  btnShowPwd->setIcon(_icClosedEye);
  btnShowPwd->setIconSize(result);

  btnShowPwd->setToolTip(ki18n("Show password"));
}

PWDialog::~PWDialog() {}

void PWDialog::krb5promptSetText(const QString &text) { krb5prompt->setText(text); }

void PWDialog::promptEditSetEchoMode(QLineEdit::EchoMode mode) { promptEdit->setEchoMode(mode); }

QString PWDialog::promptEditText() const { return promptEdit->text(); }

void PWDialog::errorLabelSetText(const QString &text) { errorLabel->setText(text); }

void PWDialog::on_btnShowPwd_clicked() {
  switch (promptEdit->echoMode()) {
    case QLineEdit::Password:
      btnShowPwd->setIcon(_icEye);
      promptEdit->setEchoMode(QLineEdit::Normal);
      btnShowPwd->setToolTip(ki18n("Hide password"));
      break;
    default:
      btnShowPwd->setIcon(_icClosedEye);
      promptEdit->setEchoMode(QLineEdit::Password);
      btnShowPwd->setToolTip(ki18n("Show password"));
      break;
  }
}
