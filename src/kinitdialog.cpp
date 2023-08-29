/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include "kinitdialog.h"

KinitDialog::KinitDialog(QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl), _icEye(":/eye.svg"), _icClosedEye(":/eye-close.svg") {
  setModal(modal);
  setAccessibleName(name);
  setupUi(this);
  changeDetailsState(Qt::Unchecked);
  changeRenewState(Qt::Checked);

  QSize size32(32, 32);
  const QList<QSize> qlistSizes = _icClosedEye.availableSizes();
  QList<QSize>::const_iterator it =
      std::lower_bound(qlistSizes.begin(), qlistSizes.end(), size32, [](const QSize &a, const QSize &b) { return a.width() < b.width(); });
  QSize result = (it != qlistSizes.end()) ? *it : size32;

  btnShowPwd->setFixedSize(result);
  btnShowPwd->setIcon(_icClosedEye);
  btnShowPwd->setIconSize(result);

  btnShowPwd->setToolTip(ki18n("Show password"));
  passwordLineEdit->setEchoMode(QLineEdit::Password);
}

void KinitDialog::errorLabelSetText(const QString &text) { errorLabel->setText(text); }

void KinitDialog::userLineEditSetText(const QString &text) { userLineEdit->setText(text); }

QString KinitDialog::userLineEditText() const { return userLineEdit->text(); }

void KinitDialog::realmLineEditSetText(const QString &text) { realmLineEdit->setText(text); }

QString KinitDialog::realmLineEditText() const { return realmLineEdit->text(); }

void KinitDialog::passwordLineEditSetFocus() { passwordLineEdit->setFocus(); }

QString KinitDialog::passwordLineEditText() { return passwordLineEdit->text(); }

void KinitDialog::forwardCheckBoxSetChecked(bool check) { forwardCheckBox->setChecked(check); }

bool KinitDialog::forwardCheckBoxIsChecked() const { return forwardCheckBox->isChecked(); }

void KinitDialog::proxyCheckBoxSetChecked(bool check) { proxyCheckBox->setChecked(check); }

bool KinitDialog::proxyCheckBoxIsChecked() const { return proxyCheckBox->isChecked(); }

void KinitDialog::lifetimeSpinBoxSetValue(int v) { lifetimeSpinBox->setValue(v); }

void KinitDialog::lifetimeUnitComboBoxSetCurrentText(const QString &text) {
  int index = lifetimeUnitComboBox->findText(text);
  lifetimeUnitComboBox->setCurrentIndex(index);
}

QString KinitDialog::lifetimeUnitComboBoxCurrentText() const { return lifetimeUnitComboBox->currentText(); }

void KinitDialog::renewtimeSpinBoxSetValue(int v) { renewtimeSpinBox->setValue(v); }

int KinitDialog::renewtimeSpinBoxValue() const { return renewtimeSpinBox->value(); }

void KinitDialog::renewUnitComboBoxSetCurrentText(const QString &text) {
  int index = renewUnitComboBox->findText(text);
  renewUnitComboBox->setCurrentIndex(index);
}

QString KinitDialog::renewUnitComboBoxCurrentText() const { return renewUnitComboBox->currentText(); }

void KinitDialog::renewCheckBoxSetChecked(bool check) { renewCheckBox->setChecked(check); }

bool KinitDialog::renewCheckBoxIsChecked() const { return renewCheckBox->isChecked(); }

bool KinitDialog::ldapServerIsEmpty() const { return lineEditLdapServer->text().isEmpty(); }

QString KinitDialog::ldapServerText() const { return lineEditLdapServer->text(); }

void KinitDialog::ldapServerSetText(const QString &text) { lineEditLdapServer->setText(text); }

KinitDialog::~KinitDialog() = default;

void KinitDialog::changeDetailsState(int state) {
  qDebug("changeDetailsState called");
  if (state == Qt::Checked) {
    detailsBox->show();
  } else if (state == Qt::Unchecked) {
    detailsBox->hide();
  }
  adjustSize();
}

void KinitDialog::changeRenewState(int state) {
  qDebug("changeRenewState called");
  if (state == Qt::Checked) {
    renewtimeSpinBox->setEnabled(true);
    renewUnitComboBox->setEnabled(true);
  } else if (state == Qt::Unchecked) {
    renewtimeSpinBox->setEnabled(false);
    renewUnitComboBox->setEnabled(false);
  }
}

void KinitDialog::on_btnShowPwd_clicked() {
  switch (passwordLineEdit->echoMode()) {
    case QLineEdit::Password:
      btnShowPwd->setIcon(_icEye);
      passwordLineEdit->setEchoMode(QLineEdit::Normal);
      btnShowPwd->setToolTip(ki18n("Hide password"));
      break;
    default:
      btnShowPwd->setIcon(_icClosedEye);
      passwordLineEdit->setEchoMode(QLineEdit::Password);
      btnShowPwd->setToolTip(ki18n("Show password"));
      break;
  }
}
