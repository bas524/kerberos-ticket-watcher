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

#include <qapplication.h>
#include <qtranslator.h>
#include <qtextcodec.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpopupmenu.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtextbrowser.h>
#include <qlistview.h>
#include <qhbox.h>
#include <qvariant.h>
#include <qframe.h>
#include <qprogressbar.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qsettings.h>
#include <qsocket.h>

#include <time.h>
#include <string.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/SM/SMlib.h>

#ifdef KeyPress
#ifndef FIXX11H_KeyPress
#define FIXX11H_KeyPress
const int XKeyPress = KeyPress;
#undef KeyPress
const int KeyPress = XKeyPress;
#endif
#undef KeyPress
#endif

#include "trayicon.h"
#include "krb5ticketwatcher.h"
#include "v5.h"
#include "mainwidget.h"
#include "pwdialog.h"
#include "pwchangedialog.h"
#include "kinitdialog.h"
#include "TicketListItem.h"

#include <sys/types.h>
#include <pwd.h>
#include <et/com_err.h>


// Atoms required for monitoring the freedesktop.org notification area
static Atom manager_atom = 0;
static Atom tray_selection_atom = 0;
Window root_window = 0;
Window tray_owner = None;

//static Atom atom_KdeNetUserTime;
static Atom kde_net_wm_user_time = 0;

Time   qt_x_last_input_time = CurrentTime;

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

void setTrayOwnerWindow(Display *dsp)
{
	/* This code is basically trying to mirror what happens in
	 * eggtrayicon.c:egg_tray_icon_update_manager_window()
	 */
	// ignore events from the old tray owner
	if (tray_owner != None)
	{
		XSelectInput(dsp, tray_owner, 0);
	}
	
	// obtain the Window handle for the new tray owner
	XGrabServer(dsp);
	tray_owner = XGetSelectionOwner(dsp, tray_selection_atom);
	
	// we have to be able to spot DestroyNotify messages on the tray owner
	if (tray_owner != None)
	{
		XSelectInput(dsp, tray_owner, StructureNotifyMask|PropertyChangeMask);
	}
	XUngrabServer(dsp);
	XFlush(dsp);
}



Ktw::Ktw( int & argc, char ** argv )
	: QApplication( argc, argv )
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
	QString transFile = QString("krb5-ticket-watcher.") + QTextCodec::locale() + ".qm";

	qDebug("translation file: " + transFile);
	
	bool ok = translator.load(transFile,
	                          "/usr/share/krb5-ticket-watcher/locales/");
	
	if(ok)
	{
		installTranslator( &translator );
		qDebug("load translation successfully");
	}
	else
	{
		qWarning("failed to load translation: "+ transFile);
	}

	/* init with translations */
	lifetimeUnit  = tr("hours");
	renewtimeUnit = tr("days");
	
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
	const int max = 20;
	Atom* atoms[max];
	char* names[max];
	Atom atoms_return[max];
	int n = 0;
	
	atoms[n] = &kde_net_wm_user_time;
	names[n++] = (char *) "_NET_WM_USER_TIME";
	atoms[n] = &manager_atom;
	names[n++] = (char *) "MANAGER";
	
	Display *dsp = qt_xdisplay();
	
	XInternAtoms( dsp, names, n, false, atoms_return );
	
	for (int i = 0; i < n; i++ )
		*atoms[i] = atoms_return[i];
	
	// get the selection type we'll use to locate the notification tray
	char buf[32];
	snprintf(buf, sizeof(buf),
	         "_NET_SYSTEM_TRAY_S%d",
	         XScreenNumberOfScreen( XDefaultScreenOfDisplay(dsp) ));
	
	tray_selection_atom = XInternAtom(dsp, buf, false);
	
	// make a note of the window handle for the root window
	root_window = QApplication::desktop()->winId();
	
	XWindowAttributes attr;
	
	// this is actually futile, since Qt overrides it at some
	// unknown point in the near future.
	XGetWindowAttributes(dsp, root_window, &attr);
	XSelectInput(dsp, root_window, attr.your_event_mask | StructureNotifyMask);
	
	setTrayOwnerWindow(dsp);
	
	QPixmap pix;
	pix.loadFromData(trayimage, sizeof( trayimage ), "PNG");
	tray = new TrayIcon(pix, "Watcher", trayMenu);
	tray->setWMDock(false);
	connect(tray, SIGNAL(clicked(const QPoint &, int)), SLOT(trayClicked(const QPoint &, int)));
	//connect(tray, SIGNAL(doubleClicked(const QPoint &)), SLOT(trayDoubleClicked()));
	//connect(tray, SIGNAL(closed()), SLOT(dockActivated()));
	//connect(this, SIGNAL(trayOwnerDied()), SLOT(dockActivated()));
	connect(this, SIGNAL(newTrayOwner()), tray, SLOT(newTrayOwner()));
	connect(this, SIGNAL(trayOwnerDied()), tray, SLOT(hide()));
	setTrayToolTip("");
	tray->show();
	
}

// private ------------------------------------------------------------------

void
Ktw::initMainWindow()
{
	MainWidget *mainWidget = new MainWidget();
	connect(mainWidget->refreshButton, SIGNAL(clicked()), this, SLOT(reReadCache()));
	setMainWidget(mainWidget);
}

// public slot --------------------------------------------------------------

void
Ktw::reReadCache()
{
	QString ret = buildCcacheInfos();
	
	if(!ret.isEmpty())
	{
		qDebug("Error: %s", ret.data());
		((MainWidget*)mainWidget())->commonLabel->setText("<qt><pre>" + ret + "</pre></qt>");
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
		
	trayMenu = new QPopupMenu(0, "Kerberos5 Ticket Watcher");

	// Popup Menu item
	trayMenu->insertItem(tr("New Ticket"),
	                     this, SLOT(kinit()));
	// Popup Menu item
	trayMenu->insertItem(tr("Renew Ticket"),
	                     this, SLOT(forceRenewCredential()));
	trayMenu->insertSeparator();
	// Popup Menu item
	//trayMenu->insertItem(tr("Help"),
	//                     this, SLOT(help()));
	// Popup Menu item
	trayMenu->insertItem(tr("Quit"),
	                     qApp, SLOT(quit()));
}


// public ------------------------------------------------------------------

void
Ktw::forceRenewCredential()
{
	qDebug("forceRenewCredential called");
	initWorkflow(1);
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
				break;
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
					break;
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
	tray->setIcon(QPixmap(path));
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
		KinitDialog *dlg = new KinitDialog(mainWidget(), "kinitDialog");

		dlg->show();

		dlg->errorLabel->setText(errorTxt);
		dlg->userLineEdit->setText(getUserName());
		dlg->realmComboBox->insertStringList(realmList);
		dlg->passwordLineEdit->setFocus();
		
		dlg->forwardCheckBox->setChecked(forwardable);
		dlg->proxyCheckBox->setChecked(proxiable);
		

		if(lifetime >= 0)
		{
			dlg->lifetimeSpinBox->setValue(lifetime);
			dlg->lifetimeUnitComboBox->setCurrentText(lifetimeUnit);
		}
		else
		{
			dlg->lifetimeSpinBox->setValue(0);
		}

		if(renewtime >= 0)
		{
			dlg->renewtimeSpinBox->setValue(renewtime);
			dlg->renewUnitComboBox->setCurrentText(renewtimeUnit);
			dlg->renewCheckBox->setChecked(true);
		}
		else
		{
			dlg->renewCheckBox->setChecked(false);
		}

		
		int ret = dlg->exec();
		if(ret == QDialog::Rejected)
			return;

		errorTxt = "";

		QString principal = dlg->userLineEdit->text() + "@" + dlg->realmComboBox->currentText();

		krb5_free_principal(kcontext, kprincipal);
		retval = krb5_parse_name(kcontext, principal,
		                         &kprincipal);
		if (retval)
		{
			qDebug("Error during parse_name: %d", retval);
			ok = true;
			errorTxt = tr("Invalid principal name");
			continue;
		}
		
		if(dlg->forwardCheckBox->isChecked())
		{
			//krb5_get_init_creds_opt_set_forwardable(&opts, 1);
			forwardable = true;
		}
		else
		{
			//krb5_get_init_creds_opt_set_forwardable(&opts, 0);
			forwardable = false;
		}
		if(dlg->proxyCheckBox->isChecked())
		{
			//krb5_get_init_creds_opt_set_proxiable(&opts, 1);
			proxiable = true;
		}
		else
		{
			//krb5_get_init_creds_opt_set_proxiable(&opts, 0);
			proxiable = false;
		}

		if(dlg->lifetimeSpinBox->value() >= 0)
		{
			lifetime = dlg->lifetimeSpinBox->value();
			lifetimeUnit = dlg->lifetimeUnitComboBox->currentText();
		}
		else
		{
			lifetime = 0;
		}

		if(!dlg->renewCheckBox->isChecked())
		{
			renewtime = -1;
		}
		else if(dlg->renewtimeSpinBox->value() >= 0)
		{
			renewtime = dlg->renewtimeSpinBox->value();
			renewtimeUnit = dlg->renewUnitComboBox->currentText();
		}
		else
		{
			renewtime = 0;
		}

		setOptions(kcontext, &opts);
		
		retval = v5::initCredential(kcontext, kprincipal,
		                            &opts, dlg->passwordLineEdit->text(),
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
					errorTxt = tr("Invalid Password");
					break;
				case KRB5KDC_ERR_KEY_EXP:
                    /* password expired */
					retval = changePassword(dlg->passwordLineEdit->text());
					if(!retval)
						ok = false;
					
					break;
				case KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN:
				case KRB5KDC_ERR_S_PRINCIPAL_UNKNOWN:
					/* principal unknown */
					errorTxt = tr("Unknown principal");
					break;
				case KRB5_REALM_CANT_RESOLVE:
					errorTxt = tr("Unknown realm");
					break;
				default:
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
Ktw::trayClicked(const QPoint &, int)
{
	qDebug("tray clicked");

	if(((MainWidget*)mainWidget())->isVisible())
	{
		((MainWidget*)mainWidget())->hide();
	}
	else
	{
		reReadCache();
		((MainWidget*)mainWidget())->show();
	}
}

// public slots ------------------------------------------------------------- 

/*
  void
  Ktw::trayDoubleClicked()
  {
  qDebug("tray double clicked");
  }
*/

// public slots ------------------------------------------------------------- 

/*
  void
  Ktw::dockActivated()
  {
  qDebug("dockActivated");
  }
*/

// ---------------------------------------------------------------------------

bool
Ktw::notify(QObject *receiver, QEvent *event)
{
	if( event->type() == QEvent::Show && receiver->isWidgetType())
	{
		QWidget* w = static_cast< QWidget* >( receiver );
		if( w->isTopLevel() && qt_x_last_input_time != CurrentTime ) // CurrentTime means no input event yet
			XChangeProperty( qt_xdisplay(), w->winId(), kde_net_wm_user_time, XA_CARDINAL,
			                32, PropModeReplace, (unsigned char*)&qt_x_last_input_time, 1 );
	}
	if( event->type() == QEvent::Hide && receiver->isWidgetType())
	{
		QWidget* w = static_cast< QWidget* >( receiver );
		if( w->isTopLevel() && w->winId() != 0 )
			XDeleteProperty( qt_xdisplay(), w->winId(), kde_net_wm_user_time );
	}
	return QApplication::notify(receiver, event);
}

// ---------------------------------------------------------------------------

bool
Ktw::x11EventFilter( XEvent *_event )
{
	switch ( _event->type ) {
		case ClientMessage:
			if (_event->xclient.window == root_window && _event->xclient.message_type == manager_atom)
			{
                // A new notification area application has
                // announced its presence
				setTrayOwnerWindow(_event->xclient.display);
				newTrayOwner();
				break;
			}
		case DestroyNotify:
			if (_event->xdestroywindow.event == tray_owner)
			{
				// there is now no known notification area.
				// We're still looking out for the MANAGER
				// message sent to the root window, at which
				// point we'll have another look to see
				// whether a notification area is available.
				tray_owner = 0;
				trayOwnerDied();
				break;
			}
			
		case ButtonPress:
		case XKeyPress:
			{
				if( _event->type == ButtonPress )
					qt_x_last_input_time = _event->xbutton.time;
				else // KeyPress
					qt_x_last_input_time = _event->xkey.time;
				QWidget *w = activeWindow();
				if( w ) {
					XChangeProperty( qt_xdisplay(), w->winId(), kde_net_wm_user_time, XA_CARDINAL,
					                32, PropModeReplace, (unsigned char*)&qt_x_last_input_time, 1 );
					/*timeval tv;
					  gettimeofday( &tv, NULL );
					  unsigned long now = tv.tv_sec * 10 + tv.tv_usec / 100000;
					  XChangeProperty(qt_xdisplay(), w->winId(),
					  atom_KdeNetUserTime, XA_CARDINAL,
					  32, PropModeReplace, (unsigned char *)&now, 1);*/
				}
				break;
			}
			
		default:
			break;
	}
	
	// process the event normally
	return false;
}


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
					errorText = tr("The password you entered is invalid");
					break;
				case KRB5KDC_ERR_KEY_EXP:
					/* password expired */
					retval = changePassword(passwd);
					if(!retval)
						repeat = false;
					
					break;
				case KRB5_KDC_UNREACH:
					/* kdc unreachable, return silently */
					repeat = false;
				default:
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
	
	PWDialog pwd(NULL, "pwdialog", true,
	             Qt::WStyle_DialogBorder | Qt::WStyle_StaysOnTop);
	pwd.krb5prompt->setText(tr("Please enter the Kerberos password for <b>%1</b>").arg(princ));
	pwd.promptEdit->setEchoMode(QLineEdit::Password);
	
	if(!errorText.isEmpty())
	{
		pwd.errorLabel->setText(errorText);
	}

	krb5_free_unparsed_name(kcontext, princ);
	
	int code = pwd.exec();
	if(code == QDialog::Rejected)
		return QString::null;
	
	return pwd.promptEdit->text();
}


int
Ktw::changePassword(const QString &oldpw)
{
	krb5_error_code retval;
	krb5_creds my_creds;
	krb5_get_init_creds_opt opts;
	QString oldPasswd = oldpw;
	
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
		retval = krb5_get_init_creds_password(kcontext, &my_creds, kprincipal,
		                                      (char*)oldPasswd.ascii(), NULL, NULL,
		                                      0, "kadmin/changepw", &opts);
		
		switch(retval)
		{
			case KRB5KDC_ERR_PREAUTH_FAILED:
			case KRB5KRB_AP_ERR_BAD_INTEGRITY:
				errorText = tr("The password you entered is invalid");
				break;
			case KRB5_KDC_UNREACH:
				/* kdc unreachable, return silently */
				krb5_free_cred_contents (kcontext, &my_creds);
				return retval;
			default:
				break;
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
		PWChangeDialog pwd(NULL, "pwchangedialog", true,
		                   Qt::WStyle_DialogBorder | Qt::WStyle_StaysOnTop);
		pwd.titleTextLabel->setText(tr("Change the password for principal <b>%1</b>").arg(principal));
		
		if(!pwEqual)
		{
			pwd.errorLabel->setText(tr("The passwords are not equal"));
		}
		
		int code = pwd.exec();
		
		if(code == QDialog::Accepted)
		{
			p1 = pwd.pwEdit1->text();
			p2 = pwd.pwEdit2->text();

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
	
	if ((retval = krb5_change_password(kcontext, &my_creds, (char*)p1.ascii(),
	                                   &result_code, &result_code_string,
	                                   &result_string)))
	{
		qDebug("changing password failed. %d", retval);
		krb5_free_cred_contents (kcontext, &my_creds);
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
		
		if( (tkt_lifetime % (60*60*24)) == 0 )
		{
			lifetime = tkt_lifetime / (60*60*24);
			lifetimeUnit = tr("days");
		}
		else if( (tkt_lifetime % (60*60)) == 0 )
		{
			lifetime = tkt_lifetime / (60*60);
			lifetimeUnit = tr("hours");
		}
		else if( (tkt_lifetime % (60)) == 0 )
		{
			lifetime = tkt_lifetime / (60);
			lifetimeUnit = tr("minutes");
		}
		else
		{
			lifetime = tkt_lifetime;
			lifetimeUnit = tr("seconds");
		}

		if(creds.times.renew_till == 0)
			renewtime = -1;
		
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
		
		if(lifetimeUnit == tr("seconds")) tkt_lifetime = lifetime;
		if(lifetimeUnit == tr("minutes")) tkt_lifetime = lifetime*60;
		if(lifetimeUnit == tr("hours")) tkt_lifetime = lifetime*60*60;
		if(lifetimeUnit == tr("days")) tkt_lifetime = lifetime*60*60*24;

		qDebug("Set lifetime to: %d", tkt_lifetime);
		
		krb5_get_init_creds_opt_set_tkt_life(opts, tkt_lifetime);
	}
	

	if(renewtime > 0)
	{
		krb5_deltat tkt_renewtime = 0;

		if(renewtimeUnit == tr("seconds")) tkt_renewtime = renewtime;
		if(renewtimeUnit == tr("minutes")) tkt_renewtime = renewtime*60;
		if(renewtimeUnit == tr("hours")) tkt_renewtime = renewtime*60*60;
		if(renewtimeUnit == tr("days")) tkt_renewtime = renewtime*60*60*24;

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

    if ((code = krb5_cc_default(kcontext, &cache)))
    {
    	errmsg += "Error while getting default ccache";
    	goto done;
    }
    
    flags = 0;                          /* turns off OPENCLOSE mode */
    if ((code = krb5_cc_set_flags(kcontext, cache, flags)))
    {
    	errmsg += QString("Error while setting cache flags (ticket cache %1:%2)")
    		.arg(krb5_cc_get_type(kcontext, cache))
    		.arg(krb5_cc_get_name(kcontext, cache));
    	
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

    ((MainWidget*)mainWidget())->commonLabel->setText(QString("<qt><b>")+
                                                      tr("Ticket cache: %1:%2")
                                                      .arg(krb5_cc_get_type(kcontext, cache))
                                                      .arg(krb5_cc_get_name(kcontext, cache)) +
                                                      QString("</b><br><b>")+
                                                      tr("Default principal: %3").arg(defname)+
                                                      QString("</b><br><br>")
                                                      );
    ((MainWidget*)mainWidget())->ticketView->clear();
    
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

    ((MainWidget*)mainWidget())->ticketView->firstChild()->setOpen(true);
    
    done = true;
    
done:
    if(defname != NULL)
    	krb5_free_unparsed_name(kcontext, defname);
    if(cache != NULL)
    	krb5_cc_close(kcontext, cache);
    
    if(!done)
    {
    	qDebug("%s", errmsg.ascii());
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
	QListView *lv = ((MainWidget*)mainWidget())->ticketView;
	QListViewItem *last = NULL;
	
	lv->setSorting(-1);
	
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
		((strcmp(name, defname))? tr(" for client %1").arg(name):QString());

	QListViewItem *after = 0;
	if( (after = lv->lastItem()) != 0)
	{
		if(after->depth() > 0)
		{
			after = after->parent();
		}
	}

	QTime time;
	krb5_timestamp expires = cred->times.endtime - v5::getNow(kcontext);
	if(expires <= 0)
		expires = 0;
	
	TicketListItem *lvi = new TicketListItem(lv, after, expires,
	                                         tr("Service principal"), n,
	                                         time.addSecs(expires).toString());

	last = new QListViewItem(lvi, tr("Valid starting"), printtime(cred->times.starttime));
	last = new QListViewItem(lvi, last,
	                         tr("Expires"), printtime(cred->times.endtime));

	if(cred->times.renew_till)
	{
		last = new QListViewItem(lvi, last,
		                         tr("Renew until"), printtime(cred->times.renew_till));
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
	
	last = new QListViewItem(lvi, last,
	                         tr("Ticket flags"), tFlags);

	retval = krb5_decode_ticket(&cred->ticket, &tkt);
	if(!retval)
	{
		last = new QListViewItem(lvi, last,
		                         tr("Key Encryption Type"), v5::etype2String(cred->keyblock.enctype));
		last = new QListViewItem(lvi, last,
		                         tr("Ticket Encryption Type" ), v5::etype2String(tkt->enc_part.enctype));
	}
	if (tkt != NULL)
		krb5_free_ticket(kcontext, tkt);

	QString addresses;
	if (!cred->addresses|| !cred->addresses[0])
	{
		addresses += tr("(none)");
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
	last = new QListViewItem(lvi, last,
	                         tr("Addresses" ), addresses);
	
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
    			return tr("Broken address (type %1 length %2)")
    				.arg(a->addrtype)
    				.arg(a->length);
    		}
    		Q_UINT32 ad;
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
    		return tr("Unknown address type %1").arg(a->addrtype);
    }
	if(addr.isNull())
	{
		return tr("(none)");
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
