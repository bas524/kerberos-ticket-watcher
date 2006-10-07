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
 * (C) Michael Calmer
 *
 * Author: Michael Calmer (Michael.Calmer at gmx.de)
 *
 * GPL Software: see COPYING for the license
 */

#include <qapplication.h>

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
	}
}

int main( int argc, char **argv )
{
	qInstallMsgHandler( myMessageOutput );
	Ktw w( argc, argv );
	//w.setGeometry( 100, 100, 200, 120 );
	//a.setMainWidget( &w );
	//w.show();
	w.initWorkflow();
	return w.exec();
}
