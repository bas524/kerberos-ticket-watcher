/*
 * krb5-ticket-watcher
 *
 * Copyright (C) 2006  Michael Calmer (Michael.Calmer at gmx.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "krb5ticketwatcher.h"

#include <pwd.h>
#include <unistd.h>

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCursor>
#include <QEvent>
#include <QHostAddress>
#include <QIcon>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QSystemTrayIcon>
#include <QDir>
#include <QList>
#include <QSettings>
#include <QStandardPaths>
#include <QTime>
#include <QToolTip>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtDebug>
#include <QStandardPaths>
#include <QSettings>
#include <QDir>
#include <QList>

#include "kinitdialog.h"
#include "krb5ccache.h"
#include "krb5creds.h"
#include "krb5credsopts.h"
#include "krb5cursor.h"
#include "krb5exception.h"
#include "krb5ticket.h"
#include "krb5timestamphelper.h"
#include "ldapinfo.h"
#include "pwchangedialog.h"
#include "pwdialog.h"
#include "version.h"

static int pw_exp;

Ktw::Ktw(int &argc, char **argv, QWidget *parent, Qt::WindowFlags fl)
    : QWidget(parent, fl), tray(nullptr), trayMenu(nullptr), waitTimer(), translator(), _context(), _principal(nullptr), tgtEndtime(0) {
  if (argc >= 3) {
    if (QString(argv[1]) == "-i" || QString(argv[1]) == "--interval") {
      bool ok = false;
      int promptInterval = QString(argv[2]).toInt(&ok);

      if (!ok) {
        qWarning("invalid value for prompt interval. Setting default.");
        promptInterval = 31;
      }
      _options.promptInterval.setTime(promptInterval);
    }
  }

  qDebug("PromptInterval is %d min.", _options.promptInterval.time());

  try {
    setDefaultOptionsUsingCreds();
  } catch (v5::Exception &ex) {
    qWarning() << "setDefaultOptionsUsingCreds error : " << ex.what();
  }
  initMainWindow();
  createTrayMenu();
  initTray();
  qApp->processEvents();

  connect(&waitTimer, SIGNAL(timeout()), this, SLOT(initWorkflow()));

  qDebug("start the timer");
  waitTimer.start(_options.promptInterval.as(ktw::TmUnit::MICROSECONDS));
}

Ktw::~Ktw() { _principal.reset(); }

// private ------------------------------------------------------------------

void Ktw::initTray() {
  QPixmap pix = generateTrayIcon(-1);
  tray = new QSystemTrayIcon(QIcon(pix), this);
  connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
  setTrayToolTip("");
  tray->setContextMenu(trayMenu);
  tray->show();
  tray->installEventFilter(this);
}

// private ------------------------------------------------------------------

void Ktw::initMainWindow() {
  setupUi(this);

  auto isFixedPitch = [](const QFont &font) {
    const QFontInfo fi(font);
    return fi.fixedPitch();
  };
  auto getMonospaceFont = [&isFixedPitch]() -> QFont {
#ifdef __APPLE__
    QFont font("Monaco");
#else
    QFont font("monospace");
#endif
    if (isFixedPitch(font)) return font;
    font.setStyleHint(QFont::Monospace);
    if (isFixedPitch(font)) return font;
    font.setStyleHint(QFont::TypeWriter);
    if (isFixedPitch(font)) return font;
    font.setFamily("courier");
    if (isFixedPitch(font)) return font;
    return font;
  };

  _monospacedFont = getMonospaceFont();

  statusBar = new QStatusBar(this);
  this->statusLayout->addWidget(statusBar, 1);
  commonLabel->setText(ki18n("Ticket cache:"));
  labelCacheV->setText(" -- ");
  labelPrincipal->setText(ki18n("Default principal:"));
  labelPrincipalV->setText(" -- ");
  _defaultStyleSheet = labelForwardableLetter->styleSheet();
  connect(refreshButton, SIGNAL(clicked()), this, SLOT(reReadCache()));
}

// public slot --------------------------------------------------------------

void Ktw::reReadCache() {
  try {
    buildCcacheInfos();
    statusBar->showMessage("cache re-read complete", 30000);
  } catch (v5::Exception &ex) {
    qWarning() << "Error: " << ex.what();
    statusBar->showMessage(ex.krb5ErrorMessage(), 30000);
  } catch (std::exception &ex) {
    qWarning() << "Error: " << ex.what();
    statusBar->showMessage(ex.what(), 30000);
  }
}

// private ------------------------------------------------------------------

void Ktw::createTrayMenu() {
  if (trayMenu) {
    return;
  }

  trayMenu = new QMenu(this);
  trayMenu->setTitle("Kerberos5 Ticket Watcher");

  kinitAction = new QAction(ki18n("&New Ticket"), this);
  kinitAction->setShortcut(ki18n("Ctrl+N"));
  kinitAction->setStatusTip(ki18n("Create a new Ticket"));
  connect(kinitAction, SIGNAL(triggered()), this, SLOT(kinit()));

  renewAction = new QAction(ki18n("&Renew Ticket"), this);
  renewAction->setShortcut(ki18n("Ctrl+R"));
  renewAction->setStatusTip(ki18n("Renew the Ticket"));
  connect(renewAction, SIGNAL(triggered()), this, SLOT(forceRenewCredential()));

  cpwAction = new QAction(ki18n("&Change Kerberos Password"), this);
  cpwAction->setShortcut(ki18n("Ctrl+C"));
  cpwAction->setStatusTip(ki18n("Change Kerberos Password"));
  connect(cpwAction, SIGNAL(triggered()), this, SLOT(changePassword()));

  destroyAction = new QAction(ki18n("&Destroy Ticket"), this);
  destroyAction->setShortcut(ki18n("Ctrl+D"));
  destroyAction->setStatusTip(ki18n("Destroy the Ticket"));
  connect(destroyAction, SIGNAL(triggered()), this, SLOT(destroyCredential()));

  restoreAction = new QAction(ki18n("Re&store"), this);
  restoreAction->setShortcut(ki18n("Ctrl+S"));
  restoreAction->setStatusTip(ki18n("Restore the Window"));
  connect(restoreAction, SIGNAL(triggered()), this, SLOT(restore()));

  quitAction = new QAction(ki18n("&Quit"), this);
  quitAction->setShortcut(ki18n("Ctrl+Q"));
  quitAction->setStatusTip(ki18n("Quit Kerberos5 Ticket Watcher"));
  connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

  versionAction = new QAction(ki18n("&Version"), this);
  versionAction->setShortcut(ki18n("Ctrl+I"));
  versionAction->setStatusTip(ki18n("Kerberos5 Ticket Watcher Version"));
  connect(versionAction, SIGNAL(triggered()), this, SLOT(version()));

  // Popup Menu item
  trayMenu->addAction(kinitAction);
  // Popup Menu item
  trayMenu->addAction(renewAction);
  // Popup Menu item
  trayMenu->addAction(destroyAction);
  // Popup Menu item
  trayMenu->addAction(cpwAction);

  trayMenu->addSeparator();
  // Popup Menu item
  trayMenu->addAction(restoreAction);
  // Popup Menu item
  trayMenu->addAction(versionAction);
  // Popup Menu item
  trayMenu->addAction(quitAction);
}

// public ------------------------------------------------------------------

void Ktw::forceRenewCredential() {
  qDebug("forceRenewCredential called");
  initWorkflow(1);
  Q_EMIT refreshButton->click();
}

void Ktw::destroyCredential() {
  if (QMessageBox::Yes == QMessageBox::question(this,
                                                // title
                                                ki18n("Destroy Kerberos Ticket Cache?"),
                                                // message text
                                                ki18n("Do you want to destroy the ticket cache?"),
                                                QMessageBox::Yes | QMessageBox::No,
                                                QMessageBox::No)) {
    int code = _context.ccache().destroy();
    if (code) {
      QMessageBox::critical(this,
                            // title
                            ki18n("Error !"),
                            // message text
                            ki18n("Ticket cache cannot be destroyed."));
    }
  } else {
    qDebug("Destroy Kerberos Ticket Cache? => No");
  }
}

void Ktw::initWorkflow(int type) {
  krb5_error_code retval = 0;
  bool isTicketExpired = false;
  bool isPromptIntervalMoreThanTicketLifeTime = true;
  QString defRealm = _context.defaultRealm();

  if (type == 69 && !defRealm.isEmpty()) {
    try {
      kinit();
      statusBar->showMessage("kinit complete", 30000);
    } catch (v5::Exception &ex) {
      statusBar->showMessage(ex.krb5ErrorMessage(), 30000);
      return;
    }
  }

  try {
    v5::Creds creds = v5::Creds::FromCCache(_context);
    isTicketExpired = ((creds.ticketEndTime() - _context.currentDateTime()) < 0);
    isPromptIntervalMoreThanTicketLifeTime = _options.promptInterval.as(ktw::TmUnit::SECONDS) >= (creds.ticketEndTime() - _context.currentDateTime());
    reReadCache();
  } catch (v5::Exception &ex) {
    qWarning() << ex.what();
    statusBar->showMessage(ex.krb5ErrorMessage(), 30000);
  }

  if (isTicketExpired && !defRealm.isEmpty()) {
    try {
      kinit();
    } catch (v5::Exception &ex) {
      statusBar->showMessage(ex.krb5ErrorMessage(), 30000);
      return;
    }
  } else if (isPromptIntervalMoreThanTicketLifeTime) {
    qDebug("stop the timer");

    waitTimer.stop();
    try {
      const QString user = getUserName();
      QString pwdKey = QString("%1_pwd").arg(user);
      QString pwd = keyChainClass.readKey(pwdKey);
      reinitCredential(pwd);
    } catch (v5::Exception &ex) {
      statusBar->showMessage(ex.krb5ErrorMessage(), 30000);
    }
    waitTimer.start(_options.promptInterval.as(ktw::TmUnit::MICROSECONDS));
  } else {
    if (type != 69) {
      auto cCache = _context.ccache();
      if (_principal != nullptr) {
        try {
          auto newCreds = cCache.renewCredentials(*_principal);
          tgtEndtime = newCreds.ticketEndTime();
          retval = 0;
          statusBar->showMessage("renew credentials complete", 30000);
        } catch (v5::Exception &ex) {
          statusBar->showMessage(ex.krb5ErrorMessage(), 30000);
          return;
        }
      } else {
        retval = KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN;
        statusBar->showMessage("unknown principal", 30000);
      }
    }

    try {
      getPwExp(QString{});
      statusBar->showMessage("password expiration info updated", 30000);
    } catch (v5::Exception &ex) {
      qWarning() << "getPwExp Error : " << ex.what();
      statusBar->showMessage(ex.krb5ErrorMessage(), 30000);
    } catch (std::exception &ex) {
      qWarning() << "getPwExp Error : " << ex.what();
      statusBar->showMessage(ex.what(), 30000);
    }
    if (!retval) tray->showMessage(ki18n("Ticket renewed"), ki18n("Ticket successfully renewed."), QSystemTrayIcon::Information, 5000);
  }
}

// public slot
// ------------------------------------------------------------------

void Ktw::setTrayToolTip(const QString &text) { tray->setToolTip(text); }

void Ktw::setTrayIcon(const QString &path) { tray->setIcon(QIcon(path)); }

void Ktw::expire_cb(
    krb5_context context, void *data, krb5_timestamp password_expiration, krb5_timestamp account_expiration, krb5_boolean is_last_req) {
  Q_UNUSED(context);
  Q_UNUSED(data);
  Q_UNUSED(account_expiration);
  Q_UNUSED(is_last_req);
  if (password_expiration != 0) {
    pw_exp = password_expiration;
  }
}

long Ktw::getPwExp(const QString &pass) {
  if (!pass.isEmpty()) {
    v5::CredsOpts opts = _context.credsOpts();
    opts.setExpireCallback(expire_cb, nullptr);
    v5::Creds creds = _context.initCreds(*_principal, opts, pass);
  }
  if (pw_exp == 0) {
    throw KRB5EXCEPTION(1, _context, "Password expiration time is 0, thats a problem");
  }

  long days = daysToPwdExpire();
  QString ts = v5::TimestampHelper::toString(pw_exp);
  QString buff = ki18n("Password expires on %1").arg(ts);

  QPixmap pixmap = generateTrayIcon(days);

  tray->setIcon(pixmap);
  tray->setToolTip(buff);
  return days;
}

long Ktw::daysToPwdExpire() {
  long days = -1;
  if (pw_exp != 0) {
    days = ((pw_exp - _context.currentDateTime()) / (60 * 60 * 24));
  }
  return days;
}

// public slots -------------------------------------------------------------

void Ktw::kinit() {
  bool ok = false;
  QString errorTxt;

  v5::CredsOpts credsOpts = _context.credsOpts();

  QString defRealm = _context.defaultRealm();
  std::unique_ptr<KinitDialog> dlg;
  bool withUI = false;
  const QString user = getUserName();
  QString principalKey = QString("%1_principal").arg(user);
  QString pwdKey = QString("%1_pwd").arg(user);
  loadOptions();
  do {
    ok = false;
    QString principal;
    QString pwd;
    if (!withUI) {
      principal = keyChainClass.readKey(principalKey);
      pwd = keyChainClass.readKey(pwdKey);
    }
    if (!principal.isEmpty() && !pwd.isEmpty()) {
      withUI = false;
    } else {
      withUI = true;
      dlg = std::make_unique<KinitDialog>(this, "kinitDialog", true);

      dlg->errorLabelSetText(errorTxt);
      QString usr = user;
      if (!principal.isEmpty()) {
        auto strs = principal.split("@");
        if (!strs.isEmpty()) {
          usr = strs.first();
        }
      }
      dlg->userLineEditSetText(usr);
      dlg->realmLineEditSetText(defRealm);
      dlg->passwordLineEditSetFocus();

      dlg->forwardCheckBoxSetChecked(_options.forwardable);
      dlg->proxyCheckBoxSetChecked(_options.proxiable);

      if (_options.lifetime.time() >= 0) {
        dlg->lifetimeSpinBoxSetValue(_options.lifetime.time());
        dlg->lifetimeUnitComboBoxSetCurrentText(_options.lifetime.unitName());
      } else {
        dlg->lifetimeSpinBoxSetValue(0);
      }

      if (_options.renewtime.time() >= 0) {
        dlg->renewtimeSpinBoxSetValue(_options.renewtime.time());
        dlg->renewUnitComboBoxSetCurrentText(_options.renewtime.unitName());
        dlg->renewCheckBoxSetChecked(true);
      } else {
        dlg->renewCheckBoxSetChecked(false);
      }

      dlg->ldapServerSetText(_options.ldapServer);

      int ret = dlg->exec();
      if (ret == QDialog::Rejected) {
        qDebug("rejected");
        throw KRB5EXCEPTION(-1, _context, "rejected pwd flow");  // TODO: May be only continue this flow?
      }
      qDebug("accepted");

      errorTxt = "";

      principal = dlg->userLineEditText() + "@" + dlg->realmLineEditText();
      pwd = dlg->passwordLineEditText();
    }

    try {
      _principal = std::make_unique<v5::Principal>(_context.principal(principal));
    } catch (std::exception &ex) {
      qWarning("Error during parse_name: %s", ex.what());
      ok = true;
      errorTxt = ki18n("Invalid principal name");
      QMessageBox::critical(this, ki18n("Failure"), errorTxt, QMessageBox::Ok, QMessageBox::Ok);
      continue;
    }

    if (withUI) {
      _options.forwardable = dlg->forwardCheckBoxIsChecked();
      _options.proxiable = dlg->proxyCheckBoxIsChecked();

      if (dlg->lifetimeSpinBoxValue() >= 0) {
        _options.lifetime.setTime(dlg->lifetimeSpinBoxValue());
        _options.lifetime.setUnit(ktw::TimeUnit::tmUnitFromText(dlg->lifetimeUnitComboBoxCurrentText()));
      }

      if (!dlg->renewCheckBoxIsChecked()) {
        _options.renewtime.setUnit(ktw::TmUnit::UNDEFINED);
      } else if (dlg->renewtimeSpinBoxValue() >= 0) {
        _options.renewtime.setTime(dlg->renewtimeSpinBoxValue());
        _options.renewtime.setUnit(ktw::TimeUnit::tmUnitFromText(dlg->renewUnitComboBoxCurrentText()));
      }
      if (!dlg->ldapServerIsEmpty()) {
        _options.ldapServer = dlg->ldapServerText();
      }
    }

    setOptions(credsOpts);

    try {
      credsOpts.setExpireCallback(expire_cb, nullptr);
      v5::Creds creds = _context.initCreds(*_principal, credsOpts, pwd);
      creds.storeInCacheFor(*_principal);
      tgtEndtime = creds.ticketEndTime();
      long days = daysToPwdExpire();
      if (days != -1) {
        QString buff = ki18n("Password expires on %1").arg(v5::TimestampHelper::toString(pw_exp));
        tray->setIcon(generateTrayIcon(days));
        tray->setToolTip(buff);
      }
      if (withUI) {
        keyChainClass.writeKey(principalKey, principal);
        keyChainClass.writeKey(pwdKey, pwd);
        saveOptions();
      }

      LdapInfo ldapinfo(_options.ldapServer, principal, pwd);
      auto dpName = QString("%1 %2 %3").arg(ldapinfo.name(), ldapinfo.middleName(), ldapinfo.surname());
      auto title = ldapinfo.title();
      if (!dpName.isEmpty() && !title.isEmpty()) {
        this->setWindowTitle(QString("%1 - %2").arg(dpName, title));
      }
    } catch (v5::Exception &ex) {
      if (ex.retval()) {
        qWarning("Error during initCredential(): %d", ex.retval());
        ok = true;
        keyChainClass.deleteKey(pwdKey);
        switch (ex.retval()) {
          case KRB5KDC_ERR_PREAUTH_FAILED:
          case KRB5KRB_AP_ERR_BAD_INTEGRITY:
            /* Invalid password, try again. */
            errorTxt = ki18n("Invalid Password");
            break;
          case KRB5KDC_ERR_KEY_EXP:
            /* password expired */
            {
              try {
                changePassword(pwd);
              } catch (std::exception &ex) {
                qWarning("kinit change password error %s", ex.what());
                ok = false;
              }
            }
            break;
          case KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN:
          case KRB5KDC_ERR_S_PRINCIPAL_UNKNOWN:
            /* principal unknown */
            errorTxt = ki18n("Unknown principal");
            break;
          case KRB5_REALM_CANT_RESOLVE:
            errorTxt = ki18n("Unknown realm");
            break;
          default:
            errorTxt = ex.krb5ErrorMessage();
            break;
        }
        if (!errorTxt.isEmpty()) {
          QMessageBox::critical(this, ki18n("Failure"), errorTxt, QMessageBox::Ok, QMessageBox::Ok);
        }
      } else {
        ok = false;
      }
    }
  } while (ok);
}

void Ktw::trayClicked(QSystemTrayIcon::ActivationReason reason) {
  if (reason == QSystemTrayIcon::Context) {
    trayMenu->exec(QCursor::pos());
  } else {
    qDebug("tray clicked");

    if (isVisible()) {
      hide();
    } else {
      restore();
    }
  }
}

void Ktw::restore() {
  reReadCache();
  show();
  activateWindow();
  raise();
}

// public slots -------------------------------------------------------------

void Ktw::reinitCredential(const QString &password) {
  QString passwd = password;

  qDebug("reinit called");

  if (_principal == nullptr) {
    qDebug("no principal found");
    _principal = std::make_unique<v5::Principal>(_context.principal(getUserName()));
  }

  v5::CredsOpts opts = _context.credsOpts();

  setOptions(opts);

  try {
    v5::Creds creds = v5::Creds::FromCCache(_context);
    qDebug("got tgt from ccache");
    tgtEndtime = creds.ticketEndTime();
  } catch (std::exception &ex) {
    qDebug("TGT expired");
    tgtEndtime = 0;
  }

  bool repeat = true;
  QString errorText;
  do {
    if (passwd.isEmpty()) {
      passwd = passwordDialog(errorText);
      if (passwd.isNull()) throw KRB5EXCEPTION(-1, _context, "Password is null");
    }

    try {
      v5::Creds creds = _context.initCreds(*_principal, opts, passwd);
      creds.storeInCacheFor(*_principal);
      tgtEndtime = creds.ticketEndTime();
      repeat = false;
    } catch (v5::Exception &ex) {
      qWarning("Error during initCredential(): %d", ex.retval());

      switch (ex.retval()) {
        case KRB5KDC_ERR_PREAUTH_FAILED:
        case KRB5KRB_AP_ERR_BAD_INTEGRITY:
          /* Invalid password, try again. */
          errorText = ki18n("The password you entered is invalid");
          break;
        case KRB5KDC_ERR_KEY_EXP:
          /* password expired */ {
            repeat = false;
            QMessageBox::warning(this, ki18n("Warning"), ki18n("Your password expired, please change it"), QMessageBox::Ok, QMessageBox::Ok);
          }
          break;
        case KRB5_KDC_UNREACH:
          /* kdc unreachable, return */
          QMessageBox::critical(this, ki18n("Failure"), ex.krb5ErrorMessage(), QMessageBox::Ok, QMessageBox::Ok);
          repeat = false;
        default:
          errorText = ex.krb5ErrorMessage();
          break;
      }
    }
    // if we repeat this task, we should ask again for the password
    if (repeat) {
      passwd.clear();
    }
  } while (repeat);

  try {
    long days = getPwExp(passwd);
    if (days < 7) {
      tray->showMessage(ki18n("Change Kerberos Password"), ki18n("Password expires after %1 days").arg(days), QSystemTrayIcon::Warning, 5000);
    }
  } catch (v5::Exception &ex) {
    tray->showMessage("Password check failed", "Can not check the password lifetime.", QSystemTrayIcon::Warning, 3000);
  }
}

QString Ktw::passwordDialog(const QString &errorText) const {
  QString princ = _principal->name();

  PWDialog pwd(nullptr, "pwdialog", true);
  pwd.krb5promptSetText(ki18n("Please enter the Kerberos password for <b>%1</b>").arg(princ));
  pwd.promptEditSetEchoMode(QLineEdit::Password);

  if (!errorText.isEmpty()) {
    pwd.errorLabelSetText(errorText);
  }

  int code = pwd.exec();
  if (code == QDialog::Rejected) return {};

  return pwd.promptEditText();
}

void Ktw::changePassword(const QString &oldpw) {
  QString oldPasswd(oldpw);

  qDebug("changePassword called");

  if (_principal == nullptr) {
    _principal = std::make_unique<v5::Principal>(_context.principal(getUserName()));
  }

  v5::CredsOpts opts = _context.credsOpts();
  opts.setTicketLifeTime(5 * 60);
  opts.setRenewLife(0);
  opts.setForwardable(false);
  opts.setProxiable(false);

  QString errorText;
  krb5_error_code retval = 0;
  std::unique_ptr<v5::Creds> creds;
  do {
    qDebug("call krb5_get_init_creds_password for kadmin/changepw");

    if (oldPasswd.isEmpty() || !errorText.isEmpty()) {
      oldPasswd = passwordDialog(errorText);

      if (oldPasswd.isNull()) return;
    }
    QString srv = "kadmin/changepw";
    try {
      creds = std::make_unique<v5::Creds>(_context.initCreds(*_principal, opts, oldPasswd, srv));
    } catch (v5::Exception &ex) {
      retval = ex.retval();
      switch (retval) {
        case KRB5KDC_ERR_PREAUTH_FAILED:
        case KRB5KRB_AP_ERR_BAD_INTEGRITY:
          errorText = ki18n("The password you entered is invalid");
          break;
        case KRB5_KDC_UNREACH:
          /* kdc unreachable, return */
          QMessageBox::critical(this, ki18n("Failure"), ex.krb5ErrorMessage(), QMessageBox::Ok, QMessageBox::Ok);
          throw KRB5EXCEPTION(retval, _context, "Can't reach service for change password");
        default:
          errorText = ex.krb5ErrorMessage();
          break;
      }
    }
  } while ((retval != 0));

  bool pwEqual = true;
  QString p1;
  QString p2;
  QString principal = _principal->name();

  do {
    PWChangeDialog pwd(nullptr, "pwchangedialog", true);
    pwd.titleTextLabelSetText(ki18n("Change the password for principal <b>%1</b>").arg(principal));

    if (!pwEqual) {
      pwd.errorLabelSetText(ki18n("The passwords are not equal"));
    }

    int code = pwd.exec();

    if (code == QDialog::Accepted) {
      p1 = pwd.pwEdit1Text();
      p2 = pwd.pwEdit2Text();

      if (p1 != p2) {
        pwEqual = false;
      } else {
        pwEqual = true;
      }
    } else {
      return;  // may be this is error
    }
  } while (!pwEqual);

  try {
    creds->changePassword(p1);
  } catch (v5::Exception &ex) {
    retval = ex.retval();
    QString msg = QString("%1 \n %2").arg(QString::fromStdString(ex.simpleWhat()), ex.krb5ErrorMessage());
    qWarning("changing password failed. %d : %s", retval, ex.what());
    QMessageBox::critical(this, ki18n("Password change failed"), msg, QMessageBox::Ok, QMessageBox::Ok);
    throw KRB5EXCEPTION(retval, _context, "changing password failed");
  }

  tray->showMessage(ki18n("Password change"), ki18n("Password successfully renewed."), QSystemTrayIcon::Information, 5000);
  const QString user = getUserName();
  QString pwdKey = QString("%1_pwd").arg(user);
  keyChainClass.deleteKey(pwdKey);
  keyChainClass.writeKey(pwdKey, p1);
  reinitCredential(p1);
}

void Ktw::setDefaultOptionsUsingCreds() {
  v5::Creds creds = v5::Creds::FromCCache(_context);
  _options.forwardable = creds.isForwardable();
  _options.proxiable = creds.isProxyable();

  auto lifeTimeDuration = creds.lifeTimeDuration();
  _options.lifetime = ktw::TimeUnit(lifeTimeDuration.first, lifeTimeDuration.second);

  auto renewTimeDuration = creds.renewTimeDuration();
  _options.renewtime = ktw::TimeUnit(renewTimeDuration.first, renewTimeDuration.second);
}

void Ktw::setOptions(v5::CredsOpts &opts) {
  qDebug("================= Options =================");
  qDebug("Forwardable %s", (_options.forwardable) ? "true" : "false");
  qDebug("Proxiable %s", (_options.proxiable) ? "true" : "false");

  opts.setForwardable(_options.forwardable);
  opts.setProxiable(_options.proxiable);

  if (_options.lifetime.time() > 0) {
    krb5_deltat tkt_lifetime = _options.lifetime.ticketTime();
    qDebug("Set lifetime to: %d", tkt_lifetime);
    opts.setTicketLifeTime(tkt_lifetime);
  }

  if (_options.renewtime.time() > 0) {
    krb5_deltat tkt_renewtime = _options.renewtime.ticketTime();
    qDebug("Set renewtime to: %d", tkt_renewtime);
    opts.setRenewLife(tkt_renewtime);
  } else if (_options.renewtime.unit() == ktw::TmUnit::UNDEFINED) {
    qDebug("Set renewtime to: %d", 0);
    opts.setRenewLife(0);
    _options.renewtime = ktw::TimeUnit(0, ktw::TmUnit::DAYS);
  } else {
    qDebug("Use default renewtime");
  }

  qDebug("================= END Options =================");

  /* This doesn't do a deep copy -- fix it later. */
  /* krb5_get_init_creds_opt_set_address_list(opts, creds->addresses); */
}

// static -----------------------------------------------------------

QString Ktw::getUserName() { return {getpwuid(getuid())->pw_name}; }

void Ktw::buildCcacheInfos() {
  ticketView->clear();

  v5::CCache cCache = _context.ccache();

  krb5_flags flags = 0; /* turns off OPENCLOSE mode */
  krb5_error_code code = cCache.setFlags(flags);
  if (code) {
    QString errmsg;
    QString tktTypeName = ki18n("Ticket cache: %1:%2").arg(cCache.type(), cCache.name());
    if (code == KRB5_FCC_NOFILE) {
      errmsg += ki18n("No credentials cache found") + " (" + tktTypeName + ")";
    } else {
      errmsg += ki18n("Error while setting cache flags") + " (" + tktTypeName + ")";
    }
    throw KRB5EXCEPTION(code, _context, errmsg);
  }
  _principal.reset();
  _principal = std::make_unique<v5::Principal>(cCache.getPrincipal());
  QString defname = _principal->name();

  commonLabel->setText(ki18n("Ticket cache:"));
  labelCacheV->setText(QString("%1:%2").arg(cCache.type(), cCache.name()));
  labelPrincipal->setText(ki18n("Default principal:"));
  labelPrincipalV->setText(defname);

  auto cursor = cCache.cursor();
  for (auto item = cursor.begin(); item != cursor.end(); ++item) {
    showCredential(*item, defname);
  }

  if (ticketView->topLevelItemCount() > 0) {
    ticketView->topLevelItem(0)->setExpanded(true);
  }
  ticketView->resizeColumnToContents(0);
  ticketView->resizeColumnToContents(1);
}

void Ktw::showCredential(v5::Creds &cred, const QString &defname) {
  QString name, sname;
  QTreeWidget *lv = ticketView;
  QTreeWidgetItem *last = nullptr;

  name = cred.clientName();
  sname = cred.serverName();

  if (!cred.hasStartTime()) cred.setStartTimeInAuthTime();

  QString n = QString(sname) + ((name == defname) ? ki18n(" for client %1").arg(name) : QString());

  QTreeWidgetItem *after = nullptr;
  int count = lv->topLevelItemCount();
  if (count > 0) {
    after = lv->topLevelItem(count - 1);
  }

  krb5_timestamp expires = cred.ticketEndTime() - _context.currentDateTime();
  if (expires <= 0) expires = 0;

  auto *lvi = new QTreeWidgetItem(lv, after);
  lvi->setText(0, ki18n("Service principal"));
  lvi->setText(1, n);
  lvi->setText(2, printInterval(expires));

  QBrush brush(QColor(128, 195, 66));
  QString brushText = "rgb(128, 195, 66)";

  if (expires == 0) {
    brush = QBrush(QColor(219, 88, 86));
    brushText = "rgb(219, 88, 86)";
  } else if (expires < 60) {
    brush = QBrush(QColor(203, 157, 6));
    brushText = "rgb(203, 157, 6)";
  }

  lvi->setBackground(0, brush);
  lvi->setBackground(1, brush);
  lvi->setBackground(2, brush);

  last = new QTreeWidgetItem(lvi);
  last->setText(0, ki18n("Valid starting"));
  last->setFont(1, _monospacedFont);
  last->setText(1, printtime(cred.ticketStartTime()));

  last = new QTreeWidgetItem(lvi, last);
  last->setText(0, ki18n("Expires"));
  last->setFont(1, _monospacedFont);
  last->setText(1, printtime(cred.ticketEndTime()));

  last = new QTreeWidgetItem(lvi, last);
  last->setText(0, ki18n("Renew until"));
  last->setFont(1, _monospacedFont);
  if (cred.ticketRenewTimeDelta() != -1) {
    last->setText(1, printtime(cred.ticketRenewTillTime()));
  } else {
    last->setText(1, "--");
  }

  QString tFlags;
  QList<char> lFlags;
  auto setLabelStyle = [&brushText, &tFlags, &lFlags, this](bool isEnabled, QLabel *label, char flag) {
    if (isEnabled) {
      tFlags += flag;
      lFlags.append(flag);
      label->setStyleSheet(QString("QLabel { background-color: %1; border-radius: 6px; }").arg(brushText));
    } else {
      label->setStyleSheet(_defaultStyleSheet);
    }
  };

  setLabelStyle(cred.isForwardable(), labelForwardableLetter, 'F');
  setLabelStyle(cred.isForwarded(), labelForwardedLetter, 'f');
  setLabelStyle(cred.isProxyable(), labelProxiableLetter, 'P');
  setLabelStyle(cred.isProxy(), labelProxyLetter, 'p');
  setLabelStyle(cred.isMayPostdate(), labelMayPostdateLetter, 'D');
  setLabelStyle(cred.isPostdated(), labelPostdateLetter, 'd');
  setLabelStyle(cred.isInvalid(), labelInvalidLetter, 'i');
  setLabelStyle(cred.isRenewable(), labelRenewableLetter, 'R');
  setLabelStyle(cred.isInitial(), labelInitialLetter, 'I');
  setLabelStyle(cred.isHWAuth(), labelHWAuthLetter, 'H');
  setLabelStyle(cred.isPreAuth(), labelPreAuthLetter, 'A');
  setLabelStyle(cred.hasTransitionPolicy(), labelTransitPolicyCheckedLetter, 'T');
  setLabelStyle(cred.isOkAsDelegate(), labelOkasDelegateLetter, 'O');
  setLabelStyle(cred.isAnonymous(), labelAnonymousLetter, 'a');

  last = new QTreeWidgetItem(lvi, last);
  last->setText(0, ki18n("Ticket flags"));
  last->setText(1, tFlags);
  QVariant itemData;
  itemData.setValue<QList<char>>(QList<char>(lFlags));
  lvi->setData(0, Qt::UserRole, itemData);
  lvi->setData(1, Qt::UserRole, brushText);

  v5::Ticket tkt = cred.ticket();

  last = new QTreeWidgetItem(lvi, last);
  last->setText(0, ki18n("Key Encryption Type"));
  last->setText(1, cred.encryptionTypeName());
  last = new QTreeWidgetItem(lvi, last);
  last->setText(0, ki18n("Ticket Encryption Type"));
  last->setText(1, tkt.encriptionTypeName());

  QStringList addresses = cred.adresses();
  if (addresses.isEmpty()) {
    addresses.append(ki18n("(none)"));
  }

  last = new QTreeWidgetItem(lvi, last);
  last->setText(0, ki18n("Addresses"));
  last->setText(1, addresses.join(", "));
}

QString Ktw::oneAddr(krb5_address *a) {
  QHostAddress addr;

  switch (a->addrtype) {
    case ADDRTYPE_INET:
      if (a->length != 4) {
      broken:
        return ki18n("Broken address (type %1 length %2)").arg(a->addrtype).arg(a->length);
      }
      {
        quint32 ad;
        memcpy(&ad, a->contents, 4);
        addr = QHostAddress(ad);
      }
      break;
    case ADDRTYPE_INET6:
      if (a->length != 16) goto broken;
      {
        Q_IPV6ADDR ad;
        memcpy(&ad, a->contents, 16);
        addr = QHostAddress(ad);
      }
      break;
    default:
      return ki18n("Unknown address type %1").arg(a->addrtype);
  }
  if (addr.isNull()) {
    return ki18n("(none)");
  }
  return addr.toString();
}

QString Ktw::printtime(time_t tv) {
  char timestring[30] = {0};
  char fill = ' ';

  if (tv == 0) {
    return "";
  }

  if (!krb5_timestamp_to_sfstring((krb5_timestamp)tv, timestring, 29, &fill)) {
    return QString::fromLocal8Bit(timestring);
  }
  return "";
}

QString Ktw::printInterval(krb5_timestamp time) {
  if (time < 1) time = 1;

  int sec = time % 60;

  time = int(time / 60);

  int min = time % 60;

  time = int(time / 60);

  int hour = time % 24;

  time = int(time / 24);

  QString str;
  if (time > 0)
    str = QString::asprintf("%d %s %02d:%02d:%02d", time, ki18n("Day(s)").toUtf8().data(), hour, min, sec);
  else
    str = QString::asprintf("%02d:%02d:%02d", hour, min, sec);

  return str;
}

// protected

bool Ktw::eventFilter(QObject *obj, QEvent *event) {
  qDebug("eventFilter called");
  if (obj == tray) {
    if (event->type() == QEvent::ToolTip) {
      QString tipText = ki18n("No Credential Cache found");
      try {
        v5::Context context;
        v5::Creds creds = v5::Creds::FromCCache(context);
        krb5_timestamp expires = creds.ticketEndTime() - context.currentDateTime();
        if (expires <= 0) expires = 0;
        tipText = ki18n("Ticket expires in %1").arg(printInterval(expires));
        setTrayToolTip(tipText);
      } catch (std::exception &e) {
        qWarning("%s", e.what());
        return false;
      }
    }
  }
  // pass the event on to the parent class
  return QWidget::eventFilter(obj, event);
}

void Ktw::paintFace(QPainter &painter, const QString &text, int iconSize, const QColor &textColor, const QColor &fillColor) {
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(fillColor));
  const int elipseSize = iconSize - 2;
  painter.drawEllipse(QRectF(1, 1, elipseSize, elipseSize));

  const int digitSize = elipseSize - 2;
  painter.setPen(textColor);
  painter.drawText(QRectF(1, 1, digitSize, digitSize), Qt::AlignCenter, text);
}

QPixmap Ktw::generateTrayIcon(long days) {
  QColor fillColor = Qt::darkGray;
  QColor textColor = Qt::white;
  if (days < 7 && days >= 0) {
    fillColor = Qt::lightGray;
    textColor = Qt::red;
  } else if (days == -1) {
    fillColor = Qt::red;
    textColor = Qt::lightGray;
  }
  int dpr = static_cast<int>(devicePixelRatioF());
  const int iconSize = 24;
  QPixmap buffer(iconSize * dpr, iconSize * dpr);
  buffer.setDevicePixelRatio(dpr);
  buffer.fill(Qt::transparent);
  QPainter painter(&buffer);
  QString value = QString("%1").arg(days);
  if (days == -1) {
    value = QString("---");
  }
  paintFace(painter, value, iconSize, textColor, fillColor);
  painter.drawPixmap(0, 0, buffer);

  return buffer;
}

void Ktw::version() {
  QString application = ki18n("Application");
  QString fullVersion = ki18n("Full Version");
  QString qtVersion = ki18n("Qt Version");
  QString buildDate = ki18n("Build Date");
  QMessageBox::about(this,
                     "About",
                     QString("<qt><table><tr><td><b>") + application + ("</b></td><td>") + ki18n("Kerberos5 Ticket Watcher") + QString("</td></tr>") +
                         QString("<tr><td><b>") + fullVersion + ("</b></td><td>") + PROJECT_VERSION + QString("</td></tr>") + QString("<tr><td><b>") +
                         qtVersion + ("</b></td><td>") + QT_VERSION_STR + QString("</td></tr>") + QString("<tr><td><b>") + buildDate +
                         ("</b></td><td>") + COMMITTER_DATE + QString("</td></tr>") + QString("</td></tr></table></qt>"));
}

void Ktw::saveOptions() {
  QString homeDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
  QString cfgdir = homeDir + "/.config/krb5tw/";
  QDir().mkpath(cfgdir);
  QString cfg = cfgdir + "options.cfg";
  QSettings settings(cfg, QSettings::IniFormat);
  auto kvProps = _options.toKeyValueProps().toStdMap();
  for (const auto &item : kvProps) {
    qDebug() << "option.key : " << item.first;
    qDebug() << "option.value : " << item.second;
    settings.setValue(item.first, item.second);
  }
  settings.sync();
}

void Ktw::loadOptions() {
  QString homeDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
  QString cfgdir = homeDir + "/.config/krb5tw/";
  QString cfg = cfgdir + "options.cfg";
  if (QFileInfo(cfg).exists()) {
    QMap<QString, QVariant> kvProps;
    QSettings settings(cfg, QSettings::IniFormat);
    auto keys = settings.allKeys();
    for (const auto &key : keys) {
      kvProps.insert(key, settings.value(key));
    }
    _options = ktw::Options::fromKeyValueProps(kvProps);
  }
}

void Ktw::on_ticketView_itemClicked(QTreeWidgetItem *item, int column) {
  QList<char> lFlags = item->data(0, Qt::UserRole).value<QList<char>>();
  QString brushText = item->data(1, Qt::UserRole).toString();
  auto getLabel = [this](char l) {
    QLabel *lbl = nullptr;
    switch (l) {
      case 'F':
        lbl = labelForwardableLetter;
        break;
      case 'f':
        lbl = labelForwardedLetter;
        break;
      case 'P':
        lbl = labelProxiableLetter;
        break;
      case 'p':
        lbl = labelProxyLetter;
        break;
      case 'D':
        lbl = labelMayPostdateLetter;
        break;
      case 'd':
        lbl = labelPostdateLetter;
        break;
      case 'i':
        lbl = labelInvalidLetter;
        break;
      case 'R':
        lbl = labelRenewableLetter;
        break;
      case 'I':
        lbl = labelInitialLetter;
        break;
      case 'H':
        lbl = labelHWAuthLetter;
        break;
      case 'A':
        lbl = labelPreAuthLetter;
        break;
      case 'T':
        lbl = labelTransitPolicyCheckedLetter;
        break;
      case 'O':
        lbl = labelOkasDelegateLetter;
        break;
      case 'a':
        lbl = labelAnonymousLetter;
        break;
      default:
        lbl = nullptr;
        break;
    }
    return lbl;
  };
  auto setLabeslStyle = [&getLabel](const QList<char> &lFlags, const QString &style) {
    for (const auto &f : lFlags) {
      QLabel *label = getLabel(f);
      if (label != nullptr) {
        label->setStyleSheet(style);
      }
    }
  };
  if (!lFlags.isEmpty() && !brushText.isEmpty()) {
    setLabeslStyle(QList<char>{'F', 'f', 'P', 'p', 'D', 'd', 'i', 'R', 'I', 'H', 'A', 'T', 'O', 'a'}, _defaultStyleSheet);
    setLabeslStyle(lFlags, QString("QLabel { background-color: %1; border-radius: 6px; }").arg(brushText));
  }
}
