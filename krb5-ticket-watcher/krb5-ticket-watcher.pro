TEMPLATE	= app
LANGUAGE	= C++

HEADERS	+= src/trayicon.h \
           src/krb5ticketwatcher.h

SOURCES	+= src/main.cpp \
	src/trayicon.cpp \
	src/trayicon_x11.cpp \
    src/krb5ticketwatcher.cpp

FORMS	= src/pwdialog.ui \
          src/pwchangedialog.ui \
          src/mainwidget.ui \
          src/kinitdialog.ui

TARGET      = krb5-ticket-watcher 

LIBS    += -lkrb5 -lcom_err

DEFINES += DEBUG

#POST_TARGETDEPS += pot qm
POST_TARGETDEPS += qm

DISTFILES += po/*.po po/Makefile

SUBDIRS += src po

QMAKE_CLEAN += src/*~

pot.target = pot
pot.commands = make -C po/ pot

qm.target = qm
qm.commands = make -C po/ all


QMAKE_EXTRA_UNIX_TARGETS += qm pot

translations.path = $(DESTDIR)/usr/share/krb5-ticket-watcher/locales/
translations.files = po/*.qm

documentation.path = $(DESTDIR)/usr/share/doc/packages/krb5-ticket-watcher/
documentation.files = COPYING

desktop.path = $(DESTDIR)/usr/share/applications/
desktop.files = krb5-ticket-watcher.desktop

binary.path = $(DESTDIR)/usr/bin/
binary.files = krb5-ticket-watcher

INSTALLS += translations desktop documentation binary

