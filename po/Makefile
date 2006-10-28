
LOCALES = krb5-ticket-watcher.de.qm \
          krb5-ticket-watcher.en.qm \
          krb5-ticket-watcher.qm
PACKAGE = krb5-ticket-watcher
VERSION = 0.1
LANGUAGES = de en

all: $(LOCALES)

%.qm: %.po
	msg2qm $< $@

pot: $(wildcard ../src/*.cpp ../src/*.h )
	xgettext -C -c -ktr -o $(PACKAGE)-$(VERSION).pot -d $(PACKAGE)-$(VERSION) $^
	for l in $(LANGUAGES); do \
		if test -f $(PACKAGE).$$l.po; then \
			mv $(PACKAGE).$$l.po $(PACKAGE)-old.$$l.po ; \
			msgmerge -o $(PACKAGE).$$l.po $(PACKAGE)-old.$$l.po $(PACKAGE)-$(VERSION).pot ; \
		else \
			cp $(PACKAGE)-$(VERSION).pot $(PACKAGE).$$l.po ; \
		fi ; \
		rm -f $(PACKAGE)-old.$$l.po ; \
	done
	cp $(PACKAGE).en.po $(PACKAGE).po

