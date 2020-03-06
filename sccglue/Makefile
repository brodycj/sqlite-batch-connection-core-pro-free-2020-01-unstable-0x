
include ../SQLite.mk

include scclib.mk

include jnibuild.mk

include ndkbuild.mk

all: ndkbuild

init: ndkinit

test: jnilib jnitest

$(SQLITE_AMALGAMATION)/sqlite3.c:
	curl -O $(SQLITE_ZIP_URL)
	unzip $(SQLITE_ZIP)

clean: jniclean ndkclean

distclean: clean ndkdistclean
	rm -rf sqlite-amalgamation
	rm -rf sqlite-amalgamation-*
	rm -rf *.zip
