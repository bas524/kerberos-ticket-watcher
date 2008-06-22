TEMPLATE	= app
LANGUAGE	= C++
APPVERSION  = 0.9.0
QT += qt3support

HEADERS	+= src/krb5ticketwatcher.h \
           src/TicketListItem.h \
           src/v5.h \
           src/kinitdialog.h \
           src/pwdialog.h \
           src/pwchangedialog.h 

SOURCES	+= src/main.cpp  \
           src/krb5ticketwatcher.cpp \
           src/TicketListItem.cpp \
           src/v5.cpp  \
           src/kinitdialog.cpp

#The following line was changed from FORMS to FORMS3 by qt3to4
FORMS	= src/pwdialog.ui \
          src/pwchangedialog.ui \
          src/mainwidget.ui \
          src/kinitdialog.ui
TRANSLATIONS = po/krb5-ticket-watcher.de.ts \
               po/krb5-ticket-watcher.en.ts

TARGET      = krb5-ticket-watcher 

LIBS    += -lkrb5 -lcom_err

DEFINES += DEBUG

#POST_TARGETDEPS += pot qm
POST_TARGETDEPS += qm

DISTFILES += po/*.ts Changes News COPYING TODO krb5-ticket-watcher.desktop

SUBDIRS += src po

QMAKE_CLEAN += src/*~ krb5-ticket-watcher-$${APPVERSION}.tar.bz2 krb5-ticket-watcher 

BZIP2 = bzip2

pot.target = pot
pot.commands = lupdate $(TARGET).pro
pot.depends = $(UICIMPLS)

qm.target = qm
qm.commands = lrelease $(TARGET).pro 

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

icon.path = $(DESTDIR)/usr/share/pixmaps/
icon.files = krb5-ticket-watcher.png

INSTALLS += translations desktop documentation icon binary

#The following line was inserted by qt3to4
QT += network  
#The following line was inserted by qt3to4
#CONFIG += uic3

