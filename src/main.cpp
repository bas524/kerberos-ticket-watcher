/*
 * Kerberos Ticket Watcher (krb5-ticket-watcher)
 *
 * Features:
 * - renew tickets in the default ticket cache
 * - re-init tickets in the default ticket cache
 *   if a renew is no longer possible
 * - init a new ticket
 * - display the default ticket cache
 * - support password change
 *
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

#include <QtGui>
#include <QApplication>
#include <QMessageBox>

#include "krb5ticketwatcher.h"
#include "krb5exception.h"
#include "stacktrace.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  switch (type) {
    case QtDebugMsg:
#ifdef DEBUG
      fprintf(stderr, "Debug: %s\n", qPrintable(msg));
#endif
      break;
    case QtWarningMsg:
      fprintf(stderr, "Warning: %s\n", qPrintable(msg));
      break;
    case QtFatalMsg:
      fprintf(stderr, "Fatal: %s\n", qPrintable(msg));
      abort();  // deliberately core dump
      break;
    case QtCriticalMsg:
      fprintf(stderr, "Critical: %s\n", qPrintable(msg));
      break;
    case QtInfoMsg:
      fprintf(stdout, "Info: %s\n", qPrintable(msg));
      break;
  }
}

int main(int argc, char **argv) {
  qInstallMessageHandler(myMessageOutput);

  QApplication app(argc, argv);

  bool systray_available = QSystemTrayIcon::isSystemTrayAvailable();
  if (!systray_available) {
    for (int i = 0; i < 20 && !systray_available; i++) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
      QThread::sleep(3);
#else
      ::sleep(3);
#endif
      systray_available = QSystemTrayIcon::isSystemTrayAvailable();
    }
    if (!systray_available) {
      QMessageBox::critical(nullptr,
                            ki18n("Systray"),
                            ki18n("I couldn't detect any system tray "
                                  "on this system."));
      return 1;
    }
  }
  QApplication::setQuitOnLastWindowClosed(false);

  try {
    Ktw w(argc, argv);
    w.initWorkflow(69);
    return app.exec();
  } catch (v5::Exception &ex) {
    if (ex.retval() != -1) {
      QMessageBox::critical(nullptr, ki18n("Failure"), ex.krb5ErrorMessage(), QMessageBox::Ok, QMessageBox::Ok);
      myMessageOutput(QtCriticalMsg, {}, QString(ex.what()));
      print_stacktrace();
    }
    return ex.retval();
  }
}
