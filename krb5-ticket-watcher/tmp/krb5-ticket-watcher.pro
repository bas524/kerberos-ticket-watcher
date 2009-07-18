TEMPLATE	= app
LANGUAGE	= C++
VERSION  = 0.9.3

HEADERS	+= src/krb5ticketwatcher.h \
           src/v5.h \
           src/kinitdialog.h \
           src/pwdialog.h \
           src/pwchangedialog.h

SOURCES	+= src/main.cpp  \
           src/krb5ticketwatcher.cpp \
           src/v5.cpp  \
           src/kinitdialog.cpp

FORMS	= src/pwdialog.ui \
          src/pwchangedialog.ui \
          src/mainwidget.ui \
          src/kinitdialog.ui
#TRANSLATIONS = po/krb5-ticket-watcher.de.ts \
#               po/krb5-ticket-watcher.en.ts

TARGET      = krb5-ticket-watcher 

LIBS    += -lkrb5 -lcom_err

DEFINES += DEBUG

#POST_TARGETDEPS += pot qm
#POST_TARGETDEPS += qm

DISTFILES += po/*.ts Changes News COPYING TODO krb5-ticket-watcher.desktop  krb5-ticket-watcher.png

SUBDIRS += src po gpo

QMAKE_CLEAN += src/*~ krb5-ticket-watcher$${VERSION}.tar.* krb5-ticket-watcher 

#pot.target = pot
#pot.commands = lupdate $(TARGET).pro
#pot.depends = $(UICIMPLS)

#qm.target = qm
#qm.commands = lrelease $(TARGET).pro 

#QMAKE_EXTRA_UNIX_TARGETS += qm pot


# change uic to use GNU gettext
QMAKE_UIC = uic -tr intl
TEXTDOMAIN = "krb5-ticket-watcher"
CATALOUGE  = "krb5-ticket-watcher.pot"
CONFIG  += mo
DEFINES += TEXTDOMAIN=\"$$TEXTDOMAIN\"

mo {
  message("in mo")
  gpo.target   = $$CATALOUGE
  gpo.depends  = $$SOURCES $$UICIMPLS
  gpo.commands = xgettext -o $@ -d $$TEXTDOMAIN --keyword=intl $^
  message(po $$po.target $$po.depends $$po.commands)
  podir.target   = gpo
  podir.depends  = $$CATALOUGE
  podir.commands = $(MAKE) -C gpo
  QMAKE_EXTRA_UNIX_TARGETS += gpo podir
}
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

translations.path = /usr/share/krb5-ticket-watcher/locales/
translations.files = po/*.qm

# it is better to install the documentation via the spec file
#documentation.path = /usr/share/doc/packages/krb5-ticket-watcher/
#documentation.files = COPYING Changes News TODO

desktop.path = /usr/share/applications/
desktop.files = krb5-ticket-watcher.desktop

binary.path = /usr/bin/
binary.files = krb5-ticket-watcher

icon.path = /usr/share/pixmaps/
icon.files = krb5-ticket-watcher.png

INSTALLS += translations desktop icon binary

QT += network  

