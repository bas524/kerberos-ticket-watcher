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

#include <QCoreApplication>

#include <QtDebug>
#include <QTextCodec>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QToolTip>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QEvent>
#include <QTime>
#include <QCursor>
#include <QSystemTrayIcon>
#include <QHostAddress>
#include <QMenu>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "krb5ticketwatcher.h"
#include "v5.h"
#include "pwdialog.h"
#include "pwchangedialog.h"
#include "kinitdialog.h"

#include <pwd.h>

static const unsigned char trayimage[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x03,
    0x86, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xad, 0x94, 0xbd, 0x6f, 0x23,
    0x45, 0x18, 0x87, 0x9f, 0x99, 0x9d, 0x9d, 0x5d, 0x7b, 0x9d, 0xe4, 0x62,
    0x8c, 0x70, 0xce, 0xb7, 0xbe, 0x20, 0xa1, 0x48, 0x01, 0x4e, 0x88, 0x86,
    0x8e, 0x0a, 0x21, 0x51, 0xf2, 0x07, 0xd0, 0x21, 0x81, 0xb7, 0x23, 0x7f,
    0x44, 0x6a, 0x5a, 0xf7, 0x14, 0xa4, 0x3d, 0x41, 0x41, 0x4b, 0x07, 0x05,
    0xba, 0x48, 0x27, 0x0e, 0x90, 0x22, 0x74, 0xb9, 0x58, 0x01, 0x87, 0xe4,
    0x72, 0xfe, 0xc0, 0xde, 0xaf, 0x99, 0xa1, 0x30, 0x4e, 0xe2, 0xc4, 0x44,
    0x08, 0xf1, 0x36, 0x3b, 0x7a, 0xf7, 0x7d, 0x9f, 0xf9, 0xcd, 0x6f, 0x3e,
    0xc4, 0xee, 0xee, 0xee, 0xe7, 0xf5, 0x7a, 0xfd, 0x33, 0xfe, 0xc7, 0xe8,
    0x74, 0x3a, 0x42, 0x35, 0x1a, 0x8d, 0x57, 0xad, 0xb5, 0x74, 0x3a, 0x9d,
    0xff, 0x0c, 0x3a, 0x3b, 0x3b, 0x23, 0x4d, 0x53, 0x5a, 0xad, 0x16, 0xdd,
    0x6e, 0x17, 0x00, 0x65, 0xad, 0x75, 0xb7, 0x35, 0x8d, 0xc7, 0x63, 0xc6,
    0xe3, 0x31, 0x00, 0x51, 0x14, 0x51, 0xab, 0xd5, 0xb0, 0xd6, 0x62, 0x8c,
    0xc1, 0x39, 0x87, 0x52, 0x8a, 0x5e, 0xaf, 0x47, 0x96, 0x65, 0xb4, 0x5a,
    0xad, 0x8b, 0x3e, 0xf5, 0x4f, 0xc0, 0x2c, 0xcb, 0x38, 0x39, 0x39, 0x41,
    0x6b, 0x8d, 0xe7, 0x79, 0x94, 0x65, 0x49, 0xbf, 0xdf, 0xa7, 0xdf, 0xef,
    0xd3, 0x6e, 0xb7, 0x11, 0x42, 0xe0, 0x9c, 0xa3, 0x28, 0x0a, 0xda, 0xed,
    0x36, 0x95, 0x4a, 0x65, 0xa1, 0x5f, 0x3a, 0xb7, 0x5c, 0xf0, 0xe9, 0xe9,
    0x29, 0x2b, 0x2b, 0x2b, 0x0c, 0x87, 0x43, 0xd2, 0x34, 0x25, 0xcb, 0xb2,
    0x8b, 0xef, 0xe1, 0xe1, 0xe1, 0x05, 0xd8, 0x5a, 0x8b, 0xd6, 0x9a, 0xa2,
    0x28, 0x28, 0x8a, 0xe2, 0x12, 0xbc, 0x0c, 0x3a, 0x1a, 0x8d, 0xa8, 0x56,
    0xab, 0xf4, 0x7a, 0x3d, 0xe2, 0x38, 0x26, 0x8e, 0x63, 0x36, 0x37, 0x37,
    0x59, 0x5f, 0x5f, 0x67, 0x30, 0x18, 0x30, 0x1c, 0x0e, 0x99, 0x4c, 0x26,
    0x00, 0xf8, 0xbe, 0xcf, 0xc1, 0xc1, 0x01, 0xfb, 0xfb, 0xfb, 0x08, 0x21,
    0x6e, 0x07, 0x5b, 0x6b, 0x99, 0x4c, 0x26, 0x18, 0x63, 0x08, 0xc3, 0xf0,
    0x42, 0x59, 0xb3, 0xd9, 0x44, 0x08, 0x71, 0xf1, 0x6f, 0x5e, 0xab, 0xb5,
    0xa6, 0xd1, 0x68, 0x2c, 0x80, 0x95, 0x73, 0x6e, 0x21, 0x01, 0xa0, 0x94,
    0x4f, 0x96, 0x5b, 0x4a, 0x3b, 0xcb, 0x3b, 0x21, 0x70, 0x08, 0x1c, 0x90,
    0xe6, 0x96, 0xe9, 0x34, 0xc7, 0x0a, 0x8d, 0x90, 0x8a, 0x22, 0x4f, 0xd9,
    0xd8, 0xd8, 0x40, 0x6b, 0x8d, 0xb5, 0xf6, 0x92, 0x71, 0x5d, 0xed, 0xf9,
    0xc1, 0x77, 0x0c, 0x8e, 0x1f, 0x91, 0xd9, 0x2a, 0x2a, 0x2b, 0xe8, 0x3f,
    0x7e, 0x36, 0x53, 0xe7, 0xc0, 0x59, 0x47, 0x34, 0xec, 0x11, 0x14, 0x39,
    0xcf, 0x7f, 0xf8, 0x05, 0xf3, 0xda, 0x7b, 0x54, 0x1b, 0x31, 0x93, 0xc9,
    0x9f, 0x8c, 0x46, 0x23, 0x9a, 0xcd, 0xe6, 0xa2, 0xe2, 0x79, 0xa4, 0x83,
    0x3f, 0x18, 0x3f, 0xf9, 0x92, 0xf6, 0xdb, 0x1f, 0x80, 0x0c, 0x11, 0x80,
    0xb1, 0x86, 0xf9, 0x7a, 0x84, 0x80, 0x56, 0xeb, 0x01, 0x78, 0x3e, 0xae,
    0x98, 0xf2, 0xeb, 0xe3, 0xaf, 0x50, 0xef, 0x7c, 0xca, 0xd3, 0xc3, 0x67,
    0x14, 0x79, 0x4e, 0x1c, 0xc7, 0xcb, 0x15, 0xe7, 0x93, 0x21, 0xfa, 0x95,
    0xb7, 0x90, 0xc5, 0x0b, 0xec, 0xf8, 0x18, 0x84, 0x40, 0x09, 0x40, 0x28,
    0x50, 0x3e, 0x48, 0x0d, 0xd2, 0x03, 0x67, 0x11, 0x2f, 0xbf, 0x49, 0xb0,
    0xda, 0x20, 0x4f, 0x27, 0xc4, 0xf7, 0xee, 0x11, 0x45, 0xd1, 0xa2, 0x9d,
    0x57, 0x15, 0x0b, 0xe9, 0x41, 0x99, 0x92, 0x1d, 0x3d, 0xc2, 0x8c, 0x7b,
    0x28, 0xa1, 0x10, 0xbe, 0x46, 0xaa, 0x2a, 0x52, 0x87, 0x08, 0x4f, 0x83,
    0x33, 0x90, 0x8f, 0x20, 0xa8, 0x21, 0x54, 0x08, 0x40, 0xbd, 0xfe, 0x12,
    0x5a, 0xfb, 0x5c, 0x65, 0xa9, 0xb9, 0xe1, 0x79, 0x69, 0x28, 0x8a, 0x12,
    0x97, 0xbf, 0x80, 0x93, 0x6f, 0x61, 0xd4, 0xc3, 0x05, 0x2b, 0x88, 0x70,
    0x1d, 0x17, 0xac, 0x82, 0x1f, 0x00, 0x06, 0x70, 0x60, 0x73, 0x98, 0x3e,
    0x40, 0x8a, 0xbb, 0xf8, 0xca, 0xa7, 0xdf, 0xef, 0x93, 0x65, 0x29, 0xdb,
    0xdb, 0xdb, 0x37, 0xad, 0xc8, 0xb3, 0x82, 0xac, 0x28, 0x90, 0xf9, 0x39,
    0xe6, 0xec, 0x47, 0x5c, 0x6a, 0x71, 0xd1, 0x14, 0x2b, 0x1c, 0x9e, 0x74,
    0x38, 0x6f, 0x0d, 0x11, 0xde, 0x01, 0xbf, 0x06, 0x08, 0xf0, 0x6b, 0x78,
    0x9e, 0x40, 0x07, 0x3e, 0xc3, 0xe3, 0x21, 0x79, 0x96, 0x2d, 0x5a, 0x31,
    0x57, 0x2c, 0x95, 0x44, 0x29, 0x85, 0x2b, 0x86, 0x94, 0xa5, 0x9d, 0x4d,
    0xa9, 0x22, 0x3c, 0x2f, 0xc2, 0xa9, 0x55, 0x5c, 0xd0, 0xc0, 0x55, 0xea,
    0xa0, 0xd7, 0xc0, 0xf3, 0x11, 0x7e, 0x88, 0xa0, 0xc4, 0x1a, 0xc3, 0xfd,
    0xf6, 0x7d, 0x2a, 0x95, 0x70, 0xb9, 0xc7, 0xce, 0x38, 0x9c, 0x17, 0x62,
    0xcb, 0x31, 0x41, 0x20, 0x31, 0xd6, 0xa2, 0x74, 0x80, 0xac, 0x54, 0x91,
    0xd5, 0x15, 0x64, 0x75, 0x15, 0xa2, 0x75, 0x08, 0xef, 0x80, 0x5e, 0xa3,
    0xfb, 0xf5, 0xec, 0x61, 0xe2, 0xfb, 0x3d, 0x96, 0x85, 0x9a, 0x5f, 0x8e,
    0xde, 0xd1, 0x53, 0x9e, 0x9f, 0x0f, 0xb0, 0xea, 0x7d, 0xaa, 0x95, 0x1a,
    0x36, 0x1f, 0x21, 0x4d, 0x15, 0x31, 0x89, 0x20, 0x0f, 0x90, 0x03, 0x0f,
    0x21, 0x7d, 0xf0, 0x4a, 0x8c, 0x3b, 0x05, 0x9a, 0x7c, 0xfc, 0x49, 0x07,
    0xe9, 0x0c, 0x52, 0x4a, 0x84, 0x10, 0x74, 0xbb, 0x5d, 0x92, 0x24, 0x79,
    0xa3, 0xd3, 0xe9, 0x5c, 0x2a, 0x8e, 0xe3, 0x98, 0x8d, 0x8d, 0x26, 0x85,
    0xdd, 0x26, 0x4b, 0x3f, 0xc4, 0x98, 0x02, 0x63, 0x0d, 0x58, 0x8b, 0xb3,
    0x06, 0xe7, 0x2c, 0x52, 0x78, 0xb3, 0xd5, 0xd9, 0x12, 0x7e, 0xff, 0x06,
    0x93, 0xa7, 0xa8, 0x20, 0xb8, 0x0a, 0x7d, 0x17, 0x78, 0xb2, 0xb0, 0x79,
    0x5a, 0x6b, 0x82, 0x20, 0x98, 0x1d, 0xbb, 0x55, 0x01, 0x42, 0x70, 0x79,
    0xd1, 0xe7, 0xa3, 0x99, 0x08, 0xfb, 0xb7, 0x98, 0x34, 0x4d, 0x09, 0x82,
    0x60, 0x0e, 0x7d, 0x1d, 0xf8, 0xe9, 0xc6, 0xa9, 0xf0, 0x3c, 0x6f, 0xa9,
    0x57, 0x57, 0x81, 0xd7, 0xa7, 0x99, 0x4e, 0xa7, 0xec, 0xed, 0xed, 0x91,
    0x24, 0xc9, 0x36, 0xf0, 0xf3, 0x82, 0xc7, 0x4a, 0x29, 0x01, 0xdc, 0x78,
    0x88, 0x6e, 0x8b, 0x79, 0xed, 0xc3, 0x87, 0x0f, 0x49, 0x92, 0xa4, 0x0d,
    0x1c, 0xdd, 0xa8, 0xd9, 0xda, 0xda, 0xfa, 0x68, 0x67, 0x67, 0xe7, 0x8b,
    0x7f, 0x4d, 0xbd, 0x12, 0x49, 0x92, 0xdc, 0x05, 0x7e, 0xbb, 0x9e, 0x77,
    0xce, 0xf1, 0x17, 0xbf, 0xc6, 0xb2, 0xcf, 0x76, 0x78, 0xf7, 0xa6, 0x00,
    0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


Ktw::Ktw( int & argc, char ** argv, QWidget* parent, Qt::WindowFlags fl )
	: QWidget( parent, fl )
	, tray(0)
	, trayMenu(0)
	, waitTimer()
	, translator()
	, kcontext(0)
	, kprincipal(0)
	, tgtEndtime(0)
	, forwardable(true)
	, proxiable(false)
	, lifetime(0)        // 0 default
	, lifetimeUnit("hours")
	, renewtime(0)       // 0 default, -1 no renewtime
	, renewtimeUnit("days")
	, promptInterval(31)  // default 31 minutes
{
	/* init with translations */
	lifetimeUnit  = ki18n("hours");
	renewtimeUnit = ki18n("days");
	
	if(argc >= 3)
	{
		if(QString(argv[1]) == "-i" ||
		   QString(argv[1]) == "--interval")
		{
			bool ok = false;
			promptInterval = QString(argv[2]).toInt(&ok);

			if(!ok)
			{
				qWarning("invalid value for prompt interval. Setting default."); 
				promptInterval = 31;
			}
		}
	}

	qDebug("PromptInterval is %d min.", promptInterval);
	
	krb5_error_code err = krb5_init_context(&kcontext);
	if (err)
	{
		qFatal("Error at krb5_init_context");
		return;
	}

	setDefaultOptionsUsingCreds(kcontext);
	initMainWindow();
	createTrayMenu();
	initTray();
	qApp->processEvents();

	connect( &waitTimer, SIGNAL(timeout()), this, SLOT(initWorkflow()) );

	qDebug("start the timer");
	waitTimer.start( promptInterval*60*1000); // retryTime is in minutes
}

Ktw::~Ktw()
{
	if(kprincipal)
		krb5_free_principal(kcontext, kprincipal);
	kprincipal = NULL;
	
	krb5_free_context(kcontext);
	kcontext = NULL;
}

// private ------------------------------------------------------------------

void
Ktw::initTray()
{
	QPixmap pix;
	pix.loadFromData(trayimage, sizeof( trayimage ), "PNG");
	tray = new QSystemTrayIcon(QIcon(pix), this);
	connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	        SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
	setTrayToolTip("");
	tray->setContextMenu ( trayMenu );
	tray->show();
	tray->installEventFilter(this);
}

// private ------------------------------------------------------------------

void
Ktw::initMainWindow()
{
	setupUi(this);

	textLabel1->setText(
						// Legend: Explain ticket flag "F"
						QString("<qt><table><tr><td><b>F</b></td><td>")+ki18n("Forwardable")+QString("</td></tr>")+
						// Legend: Explain ticket flag "f"
						QString("<tr><td><b>f</b></td><td>")+ki18n("Forwarded")+QString("</td></tr>")+
						// Legend: Explain ticket flag "p"
						QString("<tr><td><b>P</b></td><td>")+ki18n("Proxiable")+QString("</td></tr>")+
						// Legend: Explain ticket flag "P"
						QString("<tr><td><b>p</b></td><td>")+ki18n("Proxy")+QString("</td></tr>")+
						// Legend: Explain ticket flag "D"
						QString("<tr><td><b>D</b></td><td>")+ki18n("May Postdate")+QString("</td></tr>")+
						// Legend: Explain ticket flag "d"
						QString("<tr><td><b>d</b></td><td>")+ki18n("Postdated")+QString("</td></tr>")+
						// Legend: Explain ticket flag "i"
						QString("<tr><td><b>i</b></td><td>")+ki18n("Invalid")+QString("</td></tr>")+
						// Legend: Explain ticket flag "R"
						QString("<tr><td><b>R</b></td><td>")+ki18n("Renewable")+QString("</td></tr>")+
						// Legend: Explain ticket flag "I"
						QString("<tr><td><b>I</b></td><td>")+ki18n("Initial")+QString("</td></tr>")+
						// Legend: Explain ticket flag "H"
						QString("<tr><td><b>H</b></td><td>")+ki18n("HW Auth")+QString("</td></tr>")+
						// Legend: Explain ticket flag "A"
						QString("<tr><td><b>A</b></td><td>")+ki18n("Pre Auth")+QString("</td></tr>")+
						// Legend: Explain ticket flag "T"
						QString("<tr><td><b>T</b></td><td>")+ki18n("Transit Policy Checked")+QString("</td></tr>")+
						// Legend: Explain ticket flag "O"
						QString("<tr><td><b>O</b></td><td>")+ki18n("Ok as Delegate")+QString("</td></tr>")+
						// Legend: Explain ticket flag "a"
						QString("<tr><td><b>a</b></td><td>")+ki18n("Anonymous")+QString("</td></tr></table></qt>")
					   );
	
	connect(refreshButton, SIGNAL(clicked()), this, SLOT(reReadCache()));
}

// public slot --------------------------------------------------------------

void
Ktw::reReadCache()
{
	QString ret = buildCcacheInfos();
	
	if(!ret.isEmpty())
	{
		qDebug() << "Error: " << ret;
		commonLabel->setText("<qt><b>" + ret + "</b></qt>");
	}
}

// private ------------------------------------------------------------------

void
Ktw::createTrayMenu()
{
	if(trayMenu)
	{
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
	cpwAction->setStatusTip(ki18n("Change the Kerberos Password"));
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
	quitAction->setStatusTip(ki18n("Quit krb5TicketWatcher"));
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	
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
	//trayMenu->insertItem(ki18n("Help"),
	//                     this, SLOT(help()));

	// Popup Menu item
	trayMenu->addAction(restoreAction);
	// Popup Menu item
	trayMenu->addAction(quitAction);
}


// public ------------------------------------------------------------------

void
Ktw::forceRenewCredential()
{
	qDebug("forceRenewCredential called");
	initWorkflow(1);
}

void
Ktw::destroyCredential()
{
	if ( QMessageBox::Yes == QMessageBox::question(this,
												   // title
												   ki18n("Destroy Kerberos Ticket Cache?"),
												   // message text
												   ki18n("Do you want to destroy the ticket cache?"),
												   QMessageBox::Yes| QMessageBox::No, QMessageBox::No ))
	{
		int code = v5::destroyCcache(kcontext);
		if(code)
		{
			QMessageBox::critical(this,
			                      // title
			                      ki18n("Error !"),
			                      // message text
			                      ki18n("Ticket cache cannot be destroyed."));
		}
	}
	else
	{
		qDebug("Destroy Kerberos Ticket Cache? => No");
	}
}

void
Ktw::initWorkflow(int type)
{
	bool have_tgt = FALSE;
	krb5_creds creds;

	have_tgt = v5::getTgtFromCcache(kcontext, &creds);
	if (have_tgt)
	{
		krb5_copy_principal(kcontext, creds.client, &kprincipal);
		krb5_free_cred_contents (kcontext, &creds);
	}
	
	int  retval = 0;
	switch(v5::credentialCheck(kcontext, kprincipal, promptInterval, &tgtEndtime))
	{
		case renew:
			retval = v5::renewCredential(kcontext, kprincipal, &tgtEndtime);
			if(!retval)
			{
				tray->showMessage(ki18n("Ticket renewed"),
				                  ki18n("Ticket successfully renewed."),
				                  QSystemTrayIcon::Information, 5000 );
				break;
			}
		case reinit:
			qDebug("stop the timer");
			waitTimer.stop();

			retval = reinitCredential();
			
			if(retval == KRB5_KDC_UNREACH)
			{
				// cannot reach the KDC sleeping. Try next time
				qWarning("cannot reach the KDC. Sleeping ...");
				retval = 0;
			}
			
			qDebug("start the timer");
			waitTimer.start( promptInterval*60*1000); // retryTime is in minutes

			break;
		default:
			if(type != 0)
			{
				retval = v5::renewCredential(kcontext, kprincipal, &tgtEndtime);
				if(!retval)
				{
					tray->showMessage(ki18n("Ticket renewed"),
					                  ki18n("Ticket successfully renewed."),
					                  QSystemTrayIcon::Information, 5000 );
					break;
				}
			}
	}
	
	qDebug("Workflow finished");
}


// public slot ------------------------------------------------------------------


void
Ktw::setTrayToolTip(const QString& text)
{
	tray->setToolTip(text);
}

void
Ktw::setTrayIcon(const QString &path)
{
	tray->setIcon(QIcon(path));
}


// public slots ------------------------------------------------------------- 

void
Ktw::kinit()
{
	krb5_get_init_creds_opt opts;

	krb5_error_code retval;
	bool ok = false;
	QString errorTxt;
	
	krb5_get_init_creds_opt_init(&opts);

	QStringList realmList = v5::getRealms(kcontext);

	do
	{
		KinitDialog *dlg = new KinitDialog(this, "kinitDialog", true);

		//dlg->show();

		dlg->errorLabelSetText(errorTxt);
		dlg->userLineEditSetText(getUserName());
		dlg->realmComboBoxInsertStringList(realmList);
		dlg->passwordLineEditSetFocus();
		
		dlg->forwardCheckBoxSetChecked(forwardable);
		dlg->proxyCheckBoxSetChecked(proxiable);
		

		if(lifetime >= 0)
		{
			dlg->lifetimeSpinBoxSetValue(lifetime);
			dlg->lifetimeUnitComboBoxSetCurrentText(lifetimeUnit);
		}
		else
		{
			dlg->lifetimeSpinBoxSetValue(0);
		}

		if(renewtime >= 0)
		{
			dlg->renewtimeSpinBoxSetValue(renewtime);
			dlg->renewUnitComboBoxSetCurrentText(renewtimeUnit);
			dlg->renewCheckBoxSetChecked(true);
		}
		else
		{
			dlg->renewCheckBoxSetChecked(false);
		}

		
		int ret = dlg->exec();
		if(ret == QDialog::Rejected)
		{
			qDebug("rejected");
			return;
		}
		qDebug("accepted");

		errorTxt = "";

		QString principal = dlg->userLineEditText() + "@" + dlg->realmComboBoxCurrentText();

		krb5_free_principal(kcontext, kprincipal);
		retval = krb5_parse_name(kcontext, principal.toUtf8(),
		                         &kprincipal);
		if (retval)
		{
			qDebug("Error during parse_name: %d", retval);
			ok = true;
			errorTxt = ki18n("Invalid principal name");
			continue;
		}
		
		forwardable = dlg->forwardCheckBoxIsChecked();
		proxiable = dlg->proxyCheckBoxIsChecked();

		if(dlg->lifetimeSpinBoxValue() >= 0)
		{
			lifetime = dlg->lifetimeSpinBoxValue();
			lifetimeUnit = dlg->lifetimeUnitComboBoxCurrentText();
		}
		else
		{
			lifetime = 0;
		}

		if(!dlg->renewCheckBoxIsChecked())
		{
			renewtime = -1;
		}
		else if(dlg->renewtimeSpinBoxValue() >= 0)
		{
			renewtime = dlg->renewtimeSpinBoxValue();
			renewtimeUnit = dlg->renewUnitComboBoxCurrentText();
		}
		else
		{
			renewtime = 0;
		}

		setOptions(kcontext, &opts);
		
		retval = v5::initCredential(kcontext, kprincipal,
		                            &opts, dlg->passwordLineEditText(),
		                            &tgtEndtime);
		if (retval)
		{
			qDebug("Error during initCredential(): %d", retval);
			ok = true;

			switch (retval)
			{
				case KRB5KDC_ERR_PREAUTH_FAILED:
				case KRB5KRB_AP_ERR_BAD_INTEGRITY:
                    /* Invalid password, try again. */
					errorTxt = ki18n("Invalid Password");
					break;
				case KRB5KDC_ERR_KEY_EXP:
                    /* password expired */
					retval = changePassword(dlg->passwordLineEditText());
					if(!retval)
						ok = false;
					
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
					errorTxt = v5::getKrb5ErrorMessage(kcontext, retval);
					break;
			}
		}
		else
		{
			ok = false;
		}
		delete dlg;
	}
	while(ok);
}

// public slots ------------------------------------------------------------- 

/*
  void
  Ktw::help()
  {
  }
*/

// public slots ------------------------------------------------------------- 

void
Ktw::trayClicked(QSystemTrayIcon::ActivationReason reason )
{
	if(reason == QSystemTrayIcon::Context)
	{
		trayMenu->exec(QCursor::pos());
	}
	else
	{
		qDebug("tray clicked");

		if(isVisible())
		{
			hide();
		}
		else
		{
			restore();
		}
	}
}

void
Ktw::restore()
{
	reReadCache();
	show();
}

// public slots ------------------------------------------------------------- 

int
Ktw::reinitCredential(const QString& password)
{
	krb5_error_code retval;
	krb5_creds my_creds;
	krb5_get_init_creds_opt opts;

	QString passwd = password;
	
	qDebug("reinit called");
	
	if (kprincipal == NULL)
	{
		qDebug("no principal found");
		retval = krb5_parse_name(kcontext, getUserName(),
		                         &kprincipal);
		if (retval)
		{
			return retval;
		}
	}

	krb5_get_init_creds_opt_init(&opts);

	setOptions(kcontext, &opts);
	
	if (v5::getTgtFromCcache (kcontext, &my_creds))
	{
		qDebug("got tgt from ccache");
		tgtEndtime = my_creds.times.endtime;
		krb5_free_cred_contents(kcontext, &my_creds);
	}
	else
	{
		qDebug("TGT expired");
		tgtEndtime = 0;
	}

	bool repeat = true;
	QString errorText = QString::null;
	do
	{
		if(passwd == QString::null)
		{
			passwd = passwordDialog(errorText);

			if(passwd.isNull())
				return -1;
		}
		
		retval = v5::initCredential(kcontext, kprincipal, &opts, passwd, &tgtEndtime);
		
		if(retval)
		{
			qDebug("Error during initCredential(): %d", retval);
			
			switch (retval)
			{
				case KRB5KDC_ERR_PREAUTH_FAILED:
				case KRB5KRB_AP_ERR_BAD_INTEGRITY:
					/* Invalid password, try again. */
					errorText = ki18n("The password you entered is invalid");
					break;
				case KRB5KDC_ERR_KEY_EXP:
					/* password expired */
					retval = changePassword(passwd);
					if(!retval)
						repeat = false;
					
					break;
				case KRB5_KDC_UNREACH:
					/* kdc unreachable, return */
					QMessageBox::critical(this, ki18n("Failure"),
					                      v5::getKrb5ErrorMessage(kcontext, retval),
					                      QMessageBox::Ok, QMessageBox::Ok);
					repeat = false;
				default:
					errorText = v5::getKrb5ErrorMessage(kcontext, retval);
					break;
			}
		}
		else
		{
			repeat = false;
		}
		
		// if we repeat this task, we should ask again for the password
		passwd = QString::null;
	}
	while(repeat);

	return retval;
}

QString
Ktw::passwordDialog(const QString& errorText) const
{
	char* princ = NULL;
	krb5_error_code retval;
	
	if((retval = krb5_unparse_name(kcontext, kprincipal, &princ)))
	{
		qWarning("Error while unparsing principal name");
		return QString::null;
	}
	
	PWDialog pwd(NULL, "pwdialog", true);
	pwd.krb5promptSetText(ki18n("Please enter the Kerberos password for <b>%1</b>").arg(princ));
	pwd.promptEditSetEchoMode(QLineEdit::Password);
	
	if(!errorText.isEmpty())
	{
		pwd.errorLabelSetText(errorText);
	}

	krb5_free_unparsed_name(kcontext, princ);
	
	int code = pwd.exec();
	if(code == QDialog::Rejected)
		return QString::null;
	
	return pwd.promptEditText();
}


int
Ktw::changePassword(const QString &oldpw)
{
	krb5_error_code retval;
	krb5_creds my_creds;
	krb5_get_init_creds_opt opts;
	QString oldPasswd( oldpw );
	
	qDebug("changePassword called");
	
	if (kprincipal == NULL)
	{
		retval = krb5_parse_name(kcontext, getUserName(),
		                         &kprincipal);
		if (retval)
		{
			return retval;
		}
	}

	krb5_get_init_creds_opt_init(&opts);
	krb5_get_init_creds_opt_set_tkt_life(&opts, 5*60);
	krb5_get_init_creds_opt_set_renew_life(&opts, 0);
	krb5_get_init_creds_opt_set_forwardable(&opts, 0);
	krb5_get_init_creds_opt_set_proxiable(&opts, 0);

	QString errorText = QString::null;
	do
	{
		qDebug("call krb5_get_init_creds_password for kadmin/changepw");
		
		if(oldPasswd.isEmpty() || !errorText.isEmpty())
		{
			oldPasswd = passwordDialog(errorText);
			
			if(oldPasswd.isNull())
				return -1;
		}
		QString srv = "kadmin/changepw";
		retval = krb5_get_init_creds_password(kcontext,
		                                      &my_creds,
		                                      kprincipal,
		                                      oldPasswd.toUtf8().data(),
		                                      NULL,
		                                      NULL,
		                                      0,
		                                      srv.toUtf8().data(),
		                                      &opts);
		if(retval)
		{
			switch(retval)
			{
				case KRB5KDC_ERR_PREAUTH_FAILED:
				case KRB5KRB_AP_ERR_BAD_INTEGRITY:
					errorText = ki18n("The password you entered is invalid");
					break;
				case KRB5_KDC_UNREACH:
					/* kdc unreachable, return */
					krb5_free_cred_contents (kcontext, &my_creds);
					QMessageBox::critical(this, ki18n("Failure"),
					                      v5::getKrb5ErrorMessage(kcontext, retval),
					                      QMessageBox::Ok, QMessageBox::Ok);
					return retval;
				default:
					errorText = v5::getKrb5ErrorMessage(kcontext, retval);
					break;
			}
		}
	}
	while((retval != 0));
	
	bool    pwEqual = true;
	QString p1;
	QString p2;
	QString principal;
	char *princ = NULL;
	
	if((retval = krb5_unparse_name(kcontext, kprincipal, &princ)))
	{
		qWarning("Error while unparsing principal name");
		principal = "";
	}
	else
	{
		principal = QString(princ);
	}
	krb5_free_unparsed_name(kcontext, princ);

	do
	{
		PWChangeDialog pwd(NULL, "pwchangedialog", true);
		pwd.titleTextLabelSetText(ki18n("Change the password for principal <b>%1</b>").arg(principal));
		
		if(!pwEqual)
		{
			pwd.errorLabelSetText(ki18n("The passwords are not equal"));
		}
		
		int code = pwd.exec();
		
		if(code == QDialog::Accepted)
		{
			p1 = pwd.pwEdit1Text();
			p2 = pwd.pwEdit2Text();

			if(p1 != p2)
			{
				pwEqual = false;
			}
			else
			{
				pwEqual = true;
			}
		}
		else
		{
			krb5_free_cred_contents (kcontext, &my_creds);
			return retval;
		}
	}
	while(!pwEqual);

	
	int result_code;
	krb5_data result_code_string, result_string;
	
	if ((retval = krb5_change_password(kcontext, &my_creds, p1.toUtf8().data(),
	                                   &result_code, &result_code_string,
	                                   &result_string)))
	{
		qDebug("changing password failed. %d", retval);
		krb5_free_cred_contents (kcontext, &my_creds);
		QMessageBox::critical(this, ki18n("Password change failed"),
		                      v5::getKrb5ErrorMessage(kcontext, retval),
		                      QMessageBox::Ok, QMessageBox::Ok);
		return retval;
	}

	krb5_free_cred_contents (kcontext, &my_creds);
	
	if (result_code)
	{
		qDebug("%.*s%s%.*s\n",
		         (int) result_code_string.length, result_code_string.data,
		         result_string.length?": ":"",
		         (int) result_string.length,
		         result_string.data ? result_string.data : "");
		return 2;
	}
	if (result_string.data != NULL)
		free(result_string.data);
	if (result_code_string.data != NULL)
		free(result_code_string.data);
	
	return reinitCredential(p1);
}

void
Ktw::setDefaultOptionsUsingCreds(krb5_context kcontext)
{
	krb5_creds creds;
	
	if (v5::getTgtFromCcache (kcontext, &creds))
	{
		forwardable = v5::getCredForwardable(&creds) != 0;
		
		proxiable = v5::getCredProxiable(&creds) != 0;
		
 		krb5_deltat tkt_lifetime = creds.times.endtime - creds.times.starttime;
		
		if( (lifetime = (krb5_deltat) tkt_lifetime / (60*60*24)) > 0 )
		{
			lifetimeUnit = ki18n("days");
		}
		else if( (lifetime = (krb5_deltat) tkt_lifetime / (60*60)) > 0 )
		{
			lifetimeUnit = ki18n("hours");
		}
		else if( (lifetime = (krb5_deltat) tkt_lifetime / (60)) > 0 )
		{
			lifetimeUnit = ki18n("minutes");
		}
		else
		{
			lifetime = tkt_lifetime;
			lifetimeUnit = ki18n("seconds");
		}
		
		if(creds.times.renew_till == 0)
		{
			renewtime = -1;
		}
		else
		{
			krb5_deltat tkt_renewtime = creds.times.renew_till - creds.times.starttime;

			if( (renewtime = (krb5_deltat) (tkt_renewtime / (60*60*24))) > 0 )
			{
				renewtimeUnit = ki18n("days");
			}
			else if( (renewtime = (krb5_deltat) (tkt_renewtime / (60*60))) > 0 )
			{
				renewtimeUnit = ki18n("hours");
			}
			else if( (renewtime = (krb5_deltat) (tkt_renewtime / (60))) > 0 )
			{
				renewtimeUnit = ki18n("minutes");
			}
			else
			{
				renewtime = tkt_renewtime;
				renewtimeUnit = ki18n("seconds");
			}
		}
		krb5_free_cred_contents (kcontext, &creds);
	}
}

void
Ktw::setOptions(krb5_context, krb5_get_init_creds_opt *opts)
{
	qDebug("================= Options =================");
	qDebug("Forwardable %s",(forwardable)?"true":"false");
	qDebug("Proxiable %s",(proxiable)?"true":"false");
	
	krb5_get_init_creds_opt_set_forwardable(opts, (forwardable)?1:0);

	krb5_get_init_creds_opt_set_proxiable(opts, (proxiable)?1:0);


	if(lifetime > 0)
	{
		krb5_deltat tkt_lifetime = 0;
		
		if(lifetimeUnit == ki18n("seconds")) tkt_lifetime = lifetime;
		if(lifetimeUnit == ki18n("minutes")) tkt_lifetime = lifetime*60;
		if(lifetimeUnit == ki18n("hours")) tkt_lifetime = lifetime*60*60;
		if(lifetimeUnit == ki18n("days")) tkt_lifetime = lifetime*60*60*24;

		qDebug("Set lifetime to: %d", tkt_lifetime);
		
		krb5_get_init_creds_opt_set_tkt_life(opts, tkt_lifetime);
	}
	

	if(renewtime > 0)
	{
		krb5_deltat tkt_renewtime = 0;

		if(renewtimeUnit == ki18n("seconds")) tkt_renewtime = renewtime;
		if(renewtimeUnit == ki18n("minutes")) tkt_renewtime = renewtime*60;
		if(renewtimeUnit == ki18n("hours")) tkt_renewtime = renewtime*60*60;
		if(renewtimeUnit == ki18n("days")) tkt_renewtime = renewtime*60*60*24;

		qDebug("Set renewtime to: %d", tkt_renewtime);

		krb5_get_init_creds_opt_set_renew_life(opts, tkt_renewtime);
	}
	else if(renewtime < 0)
	{
		qDebug("Set renewtime to: %d", 0);

		krb5_get_init_creds_opt_set_renew_life(opts, 0);
	}
	else
	{
		qDebug("Use default renewtime");
	}
	
	qDebug("================= END Options =================");
	
	/* This doesn't do a deep copy -- fix it later. */
	/* krb5_get_init_creds_opt_set_address_list(opts, creds->addresses); */
}

// static -----------------------------------------------------------


const char *
Ktw::getUserName()
{
	return getpwuid(getuid())->pw_name;
}


QString
Ktw::buildCcacheInfos()
{
	krb5_ccache cache = NULL;
	krb5_cc_cursor cur;
	krb5_creds creds;
	krb5_flags flags;
    krb5_error_code code;
    char *defname = NULL;
    QString errmsg;
    bool done = false;

    ticketView->clear();
    
    if ((code = krb5_cc_default(kcontext, &cache)))
    {
    	errmsg += "Error while getting default ccache";
    	goto done;
    }
    
    flags = 0;                          /* turns off OPENCLOSE mode */
    if ((code = krb5_cc_set_flags(kcontext, cache, flags)))
    {
    	if (code == KRB5_FCC_NOFILE)
    	{
    		errmsg += ki18n("No credentials cache found") + " (" + ki18n("Ticket cache: %1:%2")
    			.arg(krb5_cc_get_type(kcontext, cache))
    			.arg(krb5_cc_get_name(kcontext, cache)) + ")";
    	}
    	else
    	{
    		errmsg += ki18n("Error while setting cache flags") + " (" + ki18n("Ticket cache: %1:%2")
    			.arg(krb5_cc_get_type(kcontext, cache))
    			.arg(krb5_cc_get_name(kcontext, cache)) + ")";
    	}
    	goto done;
    }
    if(kprincipal != NULL)
    	krb5_free_principal(kcontext, kprincipal);
    
    if ((code = krb5_cc_get_principal(kcontext, cache, &kprincipal)))
    {
    	errmsg += "Error while retrieving principal name";
    	goto done;
    }
    if ((code = krb5_unparse_name(kcontext, kprincipal, &defname)))
    {
    	errmsg += "Error while unparsing principal name";
    	goto done;
    }

    commonLabel->setText(QString("<qt><b>")+
                         ki18n("Ticket cache: %1:%2")
                         .arg(krb5_cc_get_type(kcontext, cache))
                         .arg(krb5_cc_get_name(kcontext, cache)) +
                         QString("</b><br><b>")+
                         ki18n("Default principal: %3").arg(defname)+
                         QString("</b><br><br>")
                         );

    if ((code = krb5_cc_start_seq_get(kcontext, cache, &cur)))
    {
    	errmsg += "Error while starting to retrieve tickets";
    	goto done;
    }
    while (!(code = krb5_cc_next_cred(kcontext, cache, &cur, &creds)))
    {
    	errmsg += showCredential(&creds, defname);
    	krb5_free_cred_contents(kcontext, &creds);
    	if(!errmsg.isEmpty())
    	{
    		goto done;
    	}
    }
    if (code == KRB5_CC_END)
    {
    	if ((code = krb5_cc_end_seq_get(kcontext, cache, &cur)))
    	{
    		errmsg += "Error while finishing ticket retrieval";
    		goto done;
    	}
    	flags = KRB5_TC_OPENCLOSE;      /* turns on OPENCLOSE mode */
    	if ((code = krb5_cc_set_flags(kcontext, cache, flags)))
    	{
    		errmsg += "Error while closing ccache";
    		goto done;
    	}
    }
    else
    {
    	errmsg += "Error while retrieving a ticket";
    	goto done;
    }

    ticketView->topLevelItem(0)->setExpanded(true);
    ticketView->resizeColumnToContents(0);
    ticketView->resizeColumnToContents(1);
    
    done = true;
    
done:
    if(defname != NULL)
    	krb5_free_unparsed_name(kcontext, defname);
    if(cache != NULL)
    	krb5_cc_close(kcontext, cache);
    
    if(!done)
    {
    	qDebug(errmsg.toUtf8());
    	return errmsg;
    }
    else
    {
    	return errmsg;
    }
}

QString
Ktw::showCredential(krb5_creds *cred, char *defname)
{
	krb5_error_code retval;
	krb5_ticket *tkt;
	char *name, *sname;
	QTreeWidget *lv = ticketView;
	QTreeWidgetItem *last = NULL;
	
	retval = krb5_unparse_name(kcontext, cred->client, &name);
	if (retval)
	{
		return "Error while unparsing client name";
	}
	retval = krb5_unparse_name(kcontext, cred->server, &sname);
	if (retval)
	{
		krb5_free_unparsed_name(kcontext, name);
		return "Error while unparsing server name";
	}
	if (!cred->times.starttime)
		cred->times.starttime = cred->times.authtime;

	QString n = QString(sname) +
		((strcmp(name, defname))? ki18n(" for client %1").arg(name):QString());

	QTreeWidgetItem *after = 0;
	int count = lv->topLevelItemCount();
	if(count > 0)
	{
		after = lv->topLevelItem( count - 1 );
	}
	/*
	  if( (after = lv->topLevelItem( count- 1 )) != 0)
	  {
	  if(after->depth() > 0)
	  {
	  after = after->parent();
	  }
	  }
	*/
	
	QTime time;
	krb5_timestamp expires = cred->times.endtime - v5::getNow(kcontext);
	if(expires <= 0)
		expires = 0;
	
	QTreeWidgetItem *lvi = new QTreeWidgetItem(lv, after);
	lvi->setText(0, ki18n("Service principal"));
	lvi->setText(1, n);
	lvi->setText(2, time.addSecs(expires).toString());

	QBrush brush(Qt::green);
	
	if( expires == 0 )
		brush = QBrush( Qt::red );
	else if(expires > 0 && expires < 60)
		brush = QBrush( Qt::yellow );
	
	lvi->setBackground(0, brush);
	lvi->setBackground(1, brush);
	lvi->setBackground(2, brush);
	
	last = new QTreeWidgetItem(lvi);
	last->setText(0, ki18n("Valid starting"));
	last->setText(1, printtime(cred->times.starttime));
	
	last = new QTreeWidgetItem(lvi, last);
	last->setText(0, ki18n("Expires"));
	last->setText(1, printtime(cred->times.endtime));

	if(cred->times.renew_till)
	{
		last = new QTreeWidgetItem(lvi, last);
		last->setText(0, ki18n("Renew until"));
		last->setText(1, printtime(cred->times.renew_till));
	}
	
	QString tFlags;
	if (v5::getCredForwardable(cred) != 0)
		tFlags += 'F';
	if (v5::getCredForwarded(cred) != 0)
		tFlags += 'f';
	if (v5::getCredProxiable(cred) != 0)
		tFlags += 'P';
	if (v5::getCredProxy(cred) != 0)
		tFlags += 'p';
	if (v5::getCredMayPostdate(cred) != 0)
		tFlags += 'D';
	if (v5::getCredPostdated(cred) != 0)
		tFlags += 'd';
	if (v5::getCredInvalid(cred) != 0)
		tFlags += 'i';
	if (v5::getCredRenewable(cred) != 0)
		tFlags += 'R';
	if (v5::getCredInitial(cred) != 0)
		tFlags += 'I';
	if (v5::getCredHwAuth(cred) != 0)
		tFlags += 'H';
	if (v5::getCredPreAuth(cred) != 0)
		tFlags += 'A';
	if (v5::getCredTransitPolicyChecked(cred) != 0)
		tFlags += 'T';
	if (v5::getCredOkAsDelegate(cred) != 0)
        tFlags += 'O';         /* D/d are taken.  Use short strings?  */
	if (v5::getCredAnonymous(cred) != 0)
		tFlags += 'a';
	
	last = new QTreeWidgetItem(lvi, last);
	last->setText(0, ki18n("Ticket flags"));
	last->setText(1, tFlags);

	retval = krb5_decode_ticket(&cred->ticket, &tkt);
	if(!retval)
	{
		last = new QTreeWidgetItem(lvi, last);
		last->setText(0, ki18n("Key Encryption Type"));
		last->setText(1, v5::etype2String(cred->keyblock.enctype));
		last = new QTreeWidgetItem(lvi, last);
		last->setText(0, ki18n("Ticket Encryption Type" ));
		last->setText(1, v5::etype2String(tkt->enc_part.enctype));
	}
	if (tkt != NULL)
		krb5_free_ticket(kcontext, tkt);

	QString addresses;
	if (!cred->addresses|| !cred->addresses[0])
	{
		addresses += ki18n("(none)");
	}
	else
	{
		int i;
		
		addresses += oneAddr(cred->addresses[0]);

		for (i=1; cred->addresses[i]; i++)
		{
			addresses += QString(", ");
			addresses += oneAddr(cred->addresses[i]);
		}
	}
	last = new QTreeWidgetItem(lvi, last);
	last->setText(0, ki18n("Addresses" ));
	last->setText(1, addresses);
	
	krb5_free_unparsed_name(kcontext, name);
	krb5_free_unparsed_name(kcontext, sname);
	last = NULL;
	lvi = NULL;
	
	return "";
}

QString
Ktw::oneAddr(krb5_address *a)
{
	QHostAddress addr;
	
	switch (a->addrtype)
	{
		case ADDRTYPE_INET:
    		if (a->length != 4)
    		{
    		broken:
    			return ki18n("Broken address (type %1 length %2)")
    				.arg(a->addrtype)
    				.arg(a->length);
    		}
    		quint32 ad;
    		memcpy (&ad, a->contents, 4);
    		addr = QHostAddress(ad);

    		break;
    	case ADDRTYPE_INET6:
    		if (a->length != 16)
    			goto broken;
    		{
    			Q_IPV6ADDR ad;
    			memcpy(&ad, a->contents, 16);
    			addr = QHostAddress(ad);
    		}
    		break;
    	default:
    		return ki18n("Unknown address type %1").arg(a->addrtype);
    }
	if(addr.isNull())
	{
		return ki18n("(none)");
	}
	return addr.toString();
}


QString
Ktw::printtime(time_t tv)
{
	char timestring[30];
	char fill = ' ';

	if(tv == 0)
	{
		return "";
	}
	
	if (!krb5_timestamp_to_sfstring((krb5_timestamp) tv,
	                                timestring, 29, &fill))
	{
		return timestring;
	}
	return "";
}


// protected

bool
Ktw::eventFilter(QObject *obj, QEvent *event)
{
	qDebug("eventFilter called");
	if(obj == tray)
	{
		if (event->type() == QEvent::ToolTip)
		{
			QString tipText = ki18n("No Credential Cache found");
			krb5_creds creds;
			krb5_context   kcontext;
			krb5_error_code err = krb5_init_context(&kcontext);
			if (err)
			{
				qWarning("Error at krb5_init_context");
				return false;
			}
			
			if (v5::getTgtFromCcache (kcontext, &creds))
			{
				QTime time;
				krb5_timestamp expires = creds.times.endtime - v5::getNow(kcontext);
				if(expires <= 0)
					expires = 0;
				tipText = ki18n("Ticket expires in %1").arg(time.addSecs(expires).toString());
				krb5_free_cred_contents (kcontext, &creds);
			}
			krb5_free_context(kcontext);
			setTrayToolTip(tipText);
		}
	}
	// pass the event on to the parent class
	return QWidget::eventFilter(obj, event);
}
