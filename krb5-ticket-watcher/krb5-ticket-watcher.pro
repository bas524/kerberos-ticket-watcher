TEMPLATE	= app
LANGUAGE	= C++
APPVERSION  = 0.1.3

HEADERS	+= src/trayicon.h \
           src/krb5ticketwatcher.h \
           src/TicketListItem.h

SOURCES	+= src/main.cpp \
	src/trayicon.cpp \
	src/trayicon_x11.cpp \
    src/krb5ticketwatcher.cpp \
    src/TicketListItem.cpp

FORMS	= src/pwdialog.ui \
          src/pwchangedialog.ui \
          src/mainwidget.ui \
          src/kinitdialog.ui

TARGET      = krb5-ticket-watcher 

LIBS    += -lkrb5 -lcom_err

#DEFINES += DEBUG

#POST_TARGETDEPS += pot qm
POST_TARGETDEPS += qm

DISTFILES += po/*.po po/Makefile Changes News COPYING TODO krb5-ticket-watcher.desktop

SUBDIRS += src po

QMAKE_CLEAN += src/*~ krb5-ticket-watcher-$${APPVERSION}.tar.bz2 krb5-ticket-watcher 

BZIP2 = bzip2

pot.target = pot
pot.commands = make -C po/ pot
pot.depends = $(UICIMPLS)

qm.target = qm
qm.commands = make -C po/ all

dist-bzip2.target = dist-bzip2
dist-bzip2.commands = @mkdir -p .tmp/krb5-ticket-watcher-$$APPVERSION && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) .tmp/krb5-ticket-watcher-$$APPVERSION/ && $(COPY_FILE) --parents src/mainwidget.ui.h .tmp/krb5-ticket-watcher-$$APPVERSION/ && $(COPY_FILE) --parents src/kinitdialog.ui.h .tmp/krb5-ticket-watcher-$$APPVERSION/ && ( cd `dirname .tmp/krb5-ticket-watcher-$$APPVERSION` && $(TAR) krb5-ticket-watcher-$${APPVERSION}.tar krb5-ticket-watcher-$$APPVERSION && $${BZIP2} krb5-ticket-watcher-$${APPVERSION}.tar ) && $(MOVE) `dirname .tmp/krb5-ticket-watcher-$$APPVERSION`/krb5-ticket-watcher-$${APPVERSION}.tar.bz2 . && $(DEL_FILE) -r .tmp/krb5-ticket-watcher-$$APPVERSION 


QMAKE_EXTRA_UNIX_TARGETS += qm pot dist-bzip2



translations.path = $(DESTDIR)/usr/share/krb5-ticket-watcher/locales/
translations.files = po/*.qm

documentation.path = $(DESTDIR)/usr/share/doc/packages/krb5-ticket-watcher/
documentation.files = COPYING Changes News TODO

desktop.path = $(DESTDIR)/usr/share/applications/
desktop.files = krb5-ticket-watcher.desktop

binary.path = $(DESTDIR)/usr/bin/
binary.files = krb5-ticket-watcher

INSTALLS += translations desktop documentation binary

