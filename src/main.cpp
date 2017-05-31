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

#include "krb5ticketwatcher.h"

void myMessageOutput( QtMsgType type, const char *msg )
{
	switch ( type ) {
		case QtDebugMsg:
#ifdef DEBUG			
			fprintf( stderr, "Debug: %s\n", msg );
#endif
			break;
		case QtWarningMsg:
			fprintf( stderr, "Warning: %s\n", msg );
			break;
		case QtFatalMsg:
			fprintf( stderr, "Fatal: %s\n", msg );
			abort();                    // deliberately core dump
			break;
		case QtCriticalMsg:
			fprintf( stderr, "Critical: %s\n", msg );
			break;
			/*
			  case QtSystemMsg:
			  fprintf( stderr, "System: %s\n", msg );
			  break;
			*/
	}
}

int main( int argc, char **argv )
{
	qInstallMsgHandler( myMessageOutput );

	QApplication app(argc, argv);

	if (!QSystemTrayIcon::isSystemTrayAvailable())
	{
		QMessageBox::critical(0, ki18n("Systray"),
		                      ki18n("I couldn't detect any system tray "
								   "on this system."));
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);

	Ktw w( argc, argv );
	w.initWorkflow();
	return app.exec();
}
