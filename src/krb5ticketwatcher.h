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

#ifndef KRB5_TICKET_WATCHER_H
#define KRB5_TICKET_WATCHER_H

#include <QString>
#include <QTimer>
#include <QTranslator>
#include <QSystemTrayIcon>

#define KRB5_PRIVATE 1
#include <krb5.h>

#include "krb5_tw_gettext.h"

#include "krb5context.h"
#include "krb5principal.h"
#include "krb5creds.h"

#include "ui_mainwidget.h"

class QAction;
class QSystemTrayIcon;
class QMenu;
class QEvent;

class Ktw : public QWidget, private Ui::MainWidget {
  Q_OBJECT

 public:
  Ktw(int &argc, char **argv, QWidget *parent = nullptr, Qt::WindowFlags fl = Qt::WindowType::Widget);
  ~Ktw() override;

  enum reqAction { none, renew, reinit };

 public slots:
  void forceRenewCredential();
  void destroyCredential();
  void initWorkflow(int type = 0);
  void trayClicked(QSystemTrayIcon::ActivationReason reason);
  void restore();
  void kinit();
  void setTrayToolTip(const QString &text);
  void setTrayIcon(const QString &);
  void reReadCache();

 protected:
  bool eventFilter(QObject *obj, QEvent *ev) override;

 private slots:
  void changePassword(const QString &oldpw = QString());

 private:
  void createTrayMenu();
  void initMainWindow();
  void initTray();
  void buildCcacheInfos();
  void showCredential(v5::Creds &cred, const QString &defname);

  QString printtime(time_t tv);
  QString oneAddr(krb5_address *a);
  QString printInterval(krb5_timestamp time);

  QString passwordDialog(const QString &errorText = QString()) const;

  static QString getUserName();

  void reinitCredential(const QString &password = QString());

  void setDefaultOptionsUsingCreds();

  void setOptions(v5::CredsOpts &opts);

  QPixmap generateTrayIcon(long days);
  void paintFace(QPainter &painter, const QString &text, int iconSize, const QColor &textColor, const QColor &fillColor);

  /// get_pw_exp
  /// \param pass
  /// \return count of days
  long get_pw_exp(const QString &pass);

  static void expire_cb(
      krb5_context context, void *data, krb5_timestamp password_expiration, krb5_timestamp account_expiration, krb5_boolean is_last_req);

  QSystemTrayIcon *tray;
  QMenu *trayMenu;

  QAction *kinitAction{};
  QAction *renewAction{};
  QAction *cpwAction{};
  QAction *destroyAction{};
  QAction *restoreAction{};
  QAction *quitAction{};

  QTimer waitTimer;
  QTranslator translator;

  //  krb5_context kcontext;
  v5::Context _context;
  std::unique_ptr<v5::Principal> _principal;
  //  krb5_principal kprincipal;
  krb5_timestamp tgtEndtime;

  bool forwardable;
  bool proxiable;

  krb5_deltat lifetime;  // 0 default
  QString lifetimeUnit;
  krb5_deltat renewtime;  // 0 default, -1 no renewtime
  QString renewtimeUnit;

  int promptInterval;
};

#endif  // KRB5_TICKET_WATCHER_H
