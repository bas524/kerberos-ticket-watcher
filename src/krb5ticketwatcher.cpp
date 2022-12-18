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
#include <QTextCodec>
#include <QTime>
#include <QToolTip>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtDebug>
#include <QStandardPaths>
#include <QSettings>
#include <QDir>

#include "kinitdialog.h"
#include "pwchangedialog.h"
#include "pwdialog.h"
#include "krb5creds.h"
#include "krb5ccache.h"
#include "krb5exception.h"
#include "krb5cursor.h"
#include "krb5ticket.h"
#include "krb5credsopts.h"
#include "krb5timestamphelper.h"
#include "version.h"

static int pw_exp;

static const unsigned char trayimage[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x03, 0x86, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xad, 0x94, 0xbd, 0x6f, 0x23,
    0x45, 0x18, 0x87, 0x9f, 0x99, 0x9d, 0x9d, 0x5d, 0x7b, 0x9d, 0xe4, 0x62, 0x8c, 0x70, 0xce, 0xb7, 0xbe, 0x20, 0xa1, 0x48, 0x01, 0x4e, 0x88, 0x86,
    0x8e, 0x0a, 0x21, 0x51, 0xf2, 0x07, 0xd0, 0x21, 0x81, 0xb7, 0x23, 0x7f, 0x44, 0x6a, 0x5a, 0xf7, 0x14, 0xa4, 0x3d, 0x41, 0x41, 0x4b, 0x07, 0x05,
    0xba, 0x48, 0x27, 0x0e, 0x90, 0x22, 0x74, 0xb9, 0x58, 0x01, 0x87, 0xe4, 0x72, 0xfe, 0xc0, 0xde, 0xaf, 0x99, 0xa1, 0x30, 0x4e, 0xe2, 0xc4, 0x44,
    0x08, 0xf1, 0x36, 0x3b, 0x7a, 0xf7, 0x7d, 0x9f, 0xf9, 0xcd, 0x6f, 0x3e, 0xc4, 0xee, 0xee, 0xee, 0xe7, 0xf5, 0x7a, 0xfd, 0x33, 0xfe, 0xc7, 0xe8,
    0x74, 0x3a, 0x42, 0x35, 0x1a, 0x8d, 0x57, 0xad, 0xb5, 0x74, 0x3a, 0x9d, 0xff, 0x0c, 0x3a, 0x3b, 0x3b, 0x23, 0x4d, 0x53, 0x5a, 0xad, 0x16, 0xdd,
    0x6e, 0x17, 0x00, 0x65, 0xad, 0x75, 0xb7, 0x35, 0x8d, 0xc7, 0x63, 0xc6, 0xe3, 0x31, 0x00, 0x51, 0x14, 0x51, 0xab, 0xd5, 0xb0, 0xd6, 0x62, 0x8c,
    0xc1, 0x39, 0x87, 0x52, 0x8a, 0x5e, 0xaf, 0x47, 0x96, 0x65, 0xb4, 0x5a, 0xad, 0x8b, 0x3e, 0xf5, 0x4f, 0xc0, 0x2c, 0xcb, 0x38, 0x39, 0x39, 0x41,
    0x6b, 0x8d, 0xe7, 0x79, 0x94, 0x65, 0x49, 0xbf, 0xdf, 0xa7, 0xdf, 0xef, 0xd3, 0x6e, 0xb7, 0x11, 0x42, 0xe0, 0x9c, 0xa3, 0x28, 0x0a, 0xda, 0xed,
    0x36, 0x95, 0x4a, 0x65, 0xa1, 0x5f, 0x3a, 0xb7, 0x5c, 0xf0, 0xe9, 0xe9, 0x29, 0x2b, 0x2b, 0x2b, 0x0c, 0x87, 0x43, 0xd2, 0x34, 0x25, 0xcb, 0xb2,
    0x8b, 0xef, 0xe1, 0xe1, 0xe1, 0x05, 0xd8, 0x5a, 0x8b, 0xd6, 0x9a, 0xa2, 0x28, 0x28, 0x8a, 0xe2, 0x12, 0xbc, 0x0c, 0x3a, 0x1a, 0x8d, 0xa8, 0x56,
    0xab, 0xf4, 0x7a, 0x3d, 0xe2, 0x38, 0x26, 0x8e, 0x63, 0x36, 0x37, 0x37, 0x59, 0x5f, 0x5f, 0x67, 0x30, 0x18, 0x30, 0x1c, 0x0e, 0x99, 0x4c, 0x26,
    0x00, 0xf8, 0xbe, 0xcf, 0xc1, 0xc1, 0x01, 0xfb, 0xfb, 0xfb, 0x08, 0x21, 0x6e, 0x07, 0x5b, 0x6b, 0x99, 0x4c, 0x26, 0x18, 0x63, 0x08, 0xc3, 0xf0,
    0x42, 0x59, 0xb3, 0xd9, 0x44, 0x08, 0x71, 0xf1, 0x6f, 0x5e, 0xab, 0xb5, 0xa6, 0xd1, 0x68, 0x2c, 0x80, 0x95, 0x73, 0x6e, 0x21, 0x01, 0xa0, 0x94,
    0x4f, 0x96, 0x5b, 0x4a, 0x3b, 0xcb, 0x3b, 0x21, 0x70, 0x08, 0x1c, 0x90, 0xe6, 0x96, 0xe9, 0x34, 0xc7, 0x0a, 0x8d, 0x90, 0x8a, 0x22, 0x4f, 0xd9,
    0xd8, 0xd8, 0x40, 0x6b, 0x8d, 0xb5, 0xf6, 0x92, 0x71, 0x5d, 0xed, 0xf9, 0xc1, 0x77, 0x0c, 0x8e, 0x1f, 0x91, 0xd9, 0x2a, 0x2a, 0x2b, 0xe8, 0x3f,
    0x7e, 0x36, 0x53, 0xe7, 0xc0, 0x59, 0x47, 0x34, 0xec, 0x11, 0x14, 0x39, 0xcf, 0x7f, 0xf8, 0x05, 0xf3, 0xda, 0x7b, 0x54, 0x1b, 0x31, 0x93, 0xc9,
    0x9f, 0x8c, 0x46, 0x23, 0x9a, 0xcd, 0xe6, 0xa2, 0xe2, 0x79, 0xa4, 0x83, 0x3f, 0x18, 0x3f, 0xf9, 0x92, 0xf6, 0xdb, 0x1f, 0x80, 0x0c, 0x11, 0x80,
    0xb1, 0x86, 0xf9, 0x7a, 0x84, 0x80, 0x56, 0xeb, 0x01, 0x78, 0x3e, 0xae, 0x98, 0xf2, 0xeb, 0xe3, 0xaf, 0x50, 0xef, 0x7c, 0xca, 0xd3, 0xc3, 0x67,
    0x14, 0x79, 0x4e, 0x1c, 0xc7, 0xcb, 0x15, 0xe7, 0x93, 0x21, 0xfa, 0x95, 0xb7, 0x90, 0xc5, 0x0b, 0xec, 0xf8, 0x18, 0x84, 0x40, 0x09, 0x40, 0x28,
    0x50, 0x3e, 0x48, 0x0d, 0xd2, 0x03, 0x67, 0x11, 0x2f, 0xbf, 0x49, 0xb0, 0xda, 0x20, 0x4f, 0x27, 0xc4, 0xf7, 0xee, 0x11, 0x45, 0xd1, 0xa2, 0x9d,
    0x57, 0x15, 0x0b, 0xe9, 0x41, 0x99, 0x92, 0x1d, 0x3d, 0xc2, 0x8c, 0x7b, 0x28, 0xa1, 0x10, 0xbe, 0x46, 0xaa, 0x2a, 0x52, 0x87, 0x08, 0x4f, 0x83,
    0x33, 0x90, 0x8f, 0x20, 0xa8, 0x21, 0x54, 0x08, 0x40, 0xbd, 0xfe, 0x12, 0x5a, 0xfb, 0x5c, 0x65, 0xa9, 0xb9, 0xe1, 0x79, 0x69, 0x28, 0x8a, 0x12,
    0x97, 0xbf, 0x80, 0x93, 0x6f, 0x61, 0xd4, 0xc3, 0x05, 0x2b, 0x88, 0x70, 0x1d, 0x17, 0xac, 0x82, 0x1f, 0x00, 0x06, 0x70, 0x60, 0x73, 0x98, 0x3e,
    0x40, 0x8a, 0xbb, 0xf8, 0xca, 0xa7, 0xdf, 0xef, 0x93, 0x65, 0x29, 0xdb, 0xdb, 0xdb, 0x37, 0xad, 0xc8, 0xb3, 0x82, 0xac, 0x28, 0x90, 0xf9, 0x39,
    0xe6, 0xec, 0x47, 0x5c, 0x6a, 0x71, 0xd1, 0x14, 0x2b, 0x1c, 0x9e, 0x74, 0x38, 0x6f, 0x0d, 0x11, 0xde, 0x01, 0xbf, 0x06, 0x08, 0xf0, 0x6b, 0x78,
    0x9e, 0x40, 0x07, 0x3e, 0xc3, 0xe3, 0x21, 0x79, 0x96, 0x2d, 0x5a, 0x31, 0x57, 0x2c, 0x95, 0x44, 0x29, 0x85, 0x2b, 0x86, 0x94, 0xa5, 0x9d, 0x4d,
    0xa9, 0x22, 0x3c, 0x2f, 0xc2, 0xa9, 0x55, 0x5c, 0xd0, 0xc0, 0x55, 0xea, 0xa0, 0xd7, 0xc0, 0xf3, 0x11, 0x7e, 0x88, 0xa0, 0xc4, 0x1a, 0xc3, 0xfd,
    0xf6, 0x7d, 0x2a, 0x95, 0x70, 0xb9, 0xc7, 0xce, 0x38, 0x9c, 0x17, 0x62, 0xcb, 0x31, 0x41, 0x20, 0x31, 0xd6, 0xa2, 0x74, 0x80, 0xac, 0x54, 0x91,
    0xd5, 0x15, 0x64, 0x75, 0x15, 0xa2, 0x75, 0x08, 0xef, 0x80, 0x5e, 0xa3, 0xfb, 0xf5, 0xec, 0x61, 0xe2, 0xfb, 0x3d, 0x96, 0x85, 0x9a, 0x5f, 0x8e,
    0xde, 0xd1, 0x53, 0x9e, 0x9f, 0x0f, 0xb0, 0xea, 0x7d, 0xaa, 0x95, 0x1a, 0x36, 0x1f, 0x21, 0x4d, 0x15, 0x31, 0x89, 0x20, 0x0f, 0x90, 0x03, 0x0f,
    0x21, 0x7d, 0xf0, 0x4a, 0x8c, 0x3b, 0x05, 0x9a, 0x7c, 0xfc, 0x49, 0x07, 0xe9, 0x0c, 0x52, 0x4a, 0x84, 0x10, 0x74, 0xbb, 0x5d, 0x92, 0x24, 0x79,
    0xa3, 0xd3, 0xe9, 0x5c, 0x2a, 0x8e, 0xe3, 0x98, 0x8d, 0x8d, 0x26, 0x85, 0xdd, 0x26, 0x4b, 0x3f, 0xc4, 0x98, 0x02, 0x63, 0x0d, 0x58, 0x8b, 0xb3,
    0x06, 0xe7, 0x2c, 0x52, 0x78, 0xb3, 0xd5, 0xd9, 0x12, 0x7e, 0xff, 0x06, 0x93, 0xa7, 0xa8, 0x20, 0xb8, 0x0a, 0x7d, 0x17, 0x78, 0xb2, 0xb0, 0x79,
    0x5a, 0x6b, 0x82, 0x20, 0x98, 0x1d, 0xbb, 0x55, 0x01, 0x42, 0x70, 0x79, 0xd1, 0xe7, 0xa3, 0x99, 0x08, 0xfb, 0xb7, 0x98, 0x34, 0x4d, 0x09, 0x82,
    0x60, 0x0e, 0x7d, 0x1d, 0xf8, 0xe9, 0xc6, 0xa9, 0xf0, 0x3c, 0x6f, 0xa9, 0x57, 0x57, 0x81, 0xd7, 0xa7, 0x99, 0x4e, 0xa7, 0xec, 0xed, 0xed, 0x91,
    0x24, 0xc9, 0x36, 0xf0, 0xf3, 0x82, 0xc7, 0x4a, 0x29, 0x01, 0xdc, 0x78, 0x88, 0x6e, 0x8b, 0x79, 0xed, 0xc3, 0x87, 0x0f, 0x49, 0x92, 0xa4, 0x0d,
    0x1c, 0xdd, 0xa8, 0xd9, 0xda, 0xda, 0xfa, 0x68, 0x67, 0x67, 0xe7, 0x8b, 0x7f, 0x4d, 0xbd, 0x12, 0x49, 0x92, 0xdc, 0x05, 0x7e, 0xbb, 0x9e, 0x77,
    0xce, 0xf1, 0x17, 0xbf, 0xc6, 0xb2, 0xcf, 0x76, 0x78, 0xf7, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};

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

  textLabel1->setText(
      // Legend: Explain ticket flag "F"
      QString("<qt><table><tr><td><b>F</b></td><td>") + ki18n("Forwardable") + QString("</td></tr>") +
      // Legend: Explain ticket flag "f"
      QString("<tr><td><b>f</b></td><td>") + ki18n("Forwarded") + QString("</td></tr>") +
      // Legend: Explain ticket flag "p"
      QString("<tr><td><b>P</b></td><td>") + ki18n("Proxiable") + QString("</td></tr>") +
      // Legend: Explain ticket flag "P"
      QString("<tr><td><b>p</b></td><td>") + ki18n("Proxy") + QString("</td></tr>") +
      // Legend: Explain ticket flag "D"
      QString("<tr><td><b>D</b></td><td>") + ki18n("May Postdate") + QString("</td></tr>") +
      // Legend: Explain ticket flag "d"
      QString("<tr><td><b>d</b></td><td>") + ki18n("Postdated") + QString("</td></tr>") +
      // Legend: Explain ticket flag "i"
      QString("<tr><td><b>i</b></td><td>") + ki18n("Invalid") + QString("</td></tr>") +
      // Legend: Explain ticket flag "R"
      QString("<tr><td><b>R</b></td><td>") + ki18n("Renewable") + QString("</td></tr>") +
      // Legend: Explain ticket flag "I"
      QString("<tr><td><b>I</b></td><td>") + ki18n("Initial") + QString("</td></tr>") +
      // Legend: Explain ticket flag "H"
      QString("<tr><td><b>H</b></td><td>") + ki18n("HW Auth") + QString("</td></tr>") +
      // Legend: Explain ticket flag "A"
      QString("<tr><td><b>A</b></td><td>") + ki18n("Pre Auth") + QString("</td></tr>") +
      // Legend: Explain ticket flag "T"
      QString("<tr><td><b>T</b></td><td>") + ki18n("Transit Policy Checked") + QString("</td></tr>") +
      // Legend: Explain ticket flag "O"
      QString("<tr><td><b>O</b></td><td>") + ki18n("Ok as Delegate") + QString("</td></tr>") +
      // Legend: Explain ticket flag "a"
      QString("<tr><td><b>a</b></td><td>") + ki18n("Anonymous") + QString("</td></tr></table></qt>"));

  connect(refreshButton, SIGNAL(clicked()), this, SLOT(reReadCache()));
}

// public slot --------------------------------------------------------------

void Ktw::reReadCache() {
  try {
    buildCcacheInfos();
  } catch (std::exception &ex) {
    qWarning() << "Error: " << ex.what();
    commonLabel->setText("<qt><b>" + QString(ex.what()) + "</b></qt>");
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
    kinit();
  }

  try {
    v5::Creds creds = v5::Creds::FromCCache(_context);
    isTicketExpired = ((creds.ticketEndTime() - _context.currentDateTime()) < 0);
    isPromptIntervalMoreThanTicketLifeTime = _options.promptInterval.as(ktw::TmUnit::SECONDS) >= (creds.ticketEndTime() - _context.currentDateTime());
    reReadCache();
  } catch (v5::Exception &ex) {
    qWarning() << ex.what();
  }

  if (isTicketExpired && !defRealm.isEmpty()) {
    kinit();
  } else if (isPromptIntervalMoreThanTicketLifeTime) {
    qDebug("stop the timer");

    waitTimer.stop();
    try {
      const QString user = getUserName();
      QString pwdKey = QString("%1_pwd").arg(user);
      QString pwd = keyChainClass.readKey(pwdKey);
      reinitCredential(pwd);
    } catch (v5::Exception &ex) {
      retval = ex.retval();
      if (retval == KRB5_KDC_UNREACH) {
        qWarning("cannot reach the KDC. Sleeping ...");
      } else {
        ex.rethrow();
      }
    }
    waitTimer.start(_options.promptInterval.as(ktw::TmUnit::MICROSECONDS));
  } else {
    if (type != 69) {
      auto cCache = _context.ccache();
      if (_principal != nullptr) {
        auto newCreds = cCache.renewCredentials(*_principal);
        tgtEndtime = newCreds.ticketEndTime();
        retval = 0;
      } else {
        retval = KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN;
      }
    }

    try {
      getPwExp(QString{});
    } catch (std::exception &ex) {
      qWarning() << "getPwExp Error : " << ex.what();
    }
    if (!retval) tray->showMessage(ki18n("Ticket renewed"), ki18n("Ticket successfully renewed."), QSystemTrayIcon::Information, 5000);
  }

  qDebug("Workflow finished");
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
      dlg->userLineEditSetText(getUserName());
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
    } catch (v5::Exception &ex) {
      if (ex.retval()) {
        qWarning("Error during initCredential(): %d", ex.retval());
        ok = true;

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
    passwd.clear();
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

      if (oldPasswd.isNull()) throw KRB5EXCEPTION(-1, _context, "old password is null");
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

  commonLabel->setText(QString("<qt><b>") + ki18n("Ticket cache: %1:%2").arg(cCache.type(), cCache.name()) + QString("</b><br><b>") +
                       ki18n("Default principal: %3").arg(defname) + QString("</b><br><br>"));

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

  QBrush brush(Qt::green);

  if (expires == 0)
    brush = QBrush(Qt::red);
  else if (expires < 60)
    brush = QBrush(Qt::yellow);

  lvi->setBackground(0, brush);
  lvi->setBackground(1, brush);
  lvi->setBackground(2, brush);

  last = new QTreeWidgetItem(lvi);
  last->setText(0, ki18n("Valid starting"));
  last->setText(1, printtime(cred.ticketStartTime()));

  last = new QTreeWidgetItem(lvi, last);
  last->setText(0, ki18n("Expires"));
  last->setText(1, printtime(cred.ticketEndTime()));

  if (cred.ticketRenewTime()) {
    last = new QTreeWidgetItem(lvi, last);
    last->setText(0, ki18n("Renew until"));
    last->setText(1, printtime(cred.ticketRenewTime()));
  }

  QString tFlags;
  if (cred.isForwardable()) tFlags += 'F';
  if (cred.isForwarded()) tFlags += 'f';
  if (cred.isProxyable()) tFlags += 'P';
  if (cred.isProxy()) tFlags += 'p';
  if (cred.isMayPostdate()) tFlags += 'D';
  if (cred.isPostdated()) tFlags += 'd';
  if (cred.isInvalid()) tFlags += 'i';
  if (cred.isRenewable()) tFlags += 'R';
  if (cred.isInitial()) tFlags += 'I';
  if (cred.isHWAuth()) tFlags += 'H';
  if (cred.isPreAuth()) tFlags += 'A';
  if (cred.hasTransitionPolicy()) tFlags += 'T';
  if (cred.isOkAsDelegate()) tFlags += 'O'; /* D/d are taken.  Use short strings?  */
  if (cred.isAnonimous()) tFlags += 'a';

  last = new QTreeWidgetItem(lvi, last);
  last->setText(0, ki18n("Ticket flags"));
  last->setText(1, tFlags);

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
  char timestring[30];
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
  auto kvProps = _options.toKeyValueProps();
  QMapIterator<QString, QVariant> i(kvProps);
  while (i.hasNext()) {
    settings.setValue(i.key(), i.value());
  }
  settings.sync();
}
