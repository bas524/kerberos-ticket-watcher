
#ifndef  KRB5_TICKET_WATCHER_H
#define  KRB5_TICKET_WATCHER_H

#include <qapplication.h>
#include <qtranslator.h>
#include <qvariant.h>
#include <qtimer.h>
#include <qmap.h>
#include <qstring.h>
#include <qthread.h>
#include <krb5.h>

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QPopupMenu;
class TrayIcon;
class MainWidget;


class Ktw : public QApplication
{
	Q_OBJECT
	
public:
	Ktw( int & argc, char ** argv );
	~Ktw();
	bool x11EventFilter( XEvent *_event );
	bool notify(QObject *receiver, QEvent *event);

	enum reqAction {none, renew, reinit};
	
	
public slots:
	void initWorkflow();
	void trayClicked(const QPoint &, int);
	//void trayDoubleClicked();
	//void dockActivated();
	void kinit();
	//void help();
	void setTrayToolTip(const QString& text);
	void setTrayIcon(const QString&);
	void reReadCache();

private slots:
	int
	renewCredential();
	
private:
	void createTrayMenu();
	void initMainWindow();
	void initTray();
	QString buildCcacheInfos();
	QString showCredential(krb5_creds *cred, char *defname);

	QString printtime(time_t tv);
	QString etype2String(krb5_enctype enctype);
	QString oneAddr(krb5_address *a);

	QString
	passwordDialog(const QString& errorText = QString::null) const;
	
	bool getTgtFromCcache(krb5_context context, krb5_creds *creds);

	int
	credentialCheck();

	static size_t
	getPrincipalRealmLength(krb5_principal p);

	static const char *
	getPrincipalRealmData(krb5_principal p);

	static const char *
	getUserName();

	int
	initCredential(krb5_get_init_creds_opt *opts,
	               const QString& password = QString::null);

	int
	reinitCredential(const QString &password = QString::null);

	int
	changePassword(const QString& oldpw = QString::null);
	
	void
	setOptionsUsingCreds(krb5_context context,
	                     krb5_creds *creds,
	                     krb5_get_init_creds_opt *opts);

	TrayIcon     *tray;
	QPopupMenu   *trayMenu;
	
	QTimer        waitTimer;
	QTranslator   translator;

	krb5_context   kcontext;
	krb5_principal kprincipal;
	krb5_timestamp creds_expiry;
	krb5_deltat    renew_lifetime;
	
	int            promptInterval;
	
signals:
	void newTrayOwner();
	void trayOwnerDied();
};

#endif //KRB5_TICKET_WATCHER_H
