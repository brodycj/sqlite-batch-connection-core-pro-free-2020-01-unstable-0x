include ../sqlite-download.mk

include scclib.mk

include jni-glue-regen.mk

include jnibuild.mk

include ndk-jar-build.mk

all: ndkbuild

test: jnilib jnitest

$(SQLITE_AMALGAMATION)/sqlite3.c:
	curl -O $(SQLITE_ZIP_URL)
	unzip $(SQLITE_ZIP)

clean: jniclean ndkclean

distclean: clean ndkdistclean glue-regen-distclean
	rm -rf sqlite-amalgamation
	rm -rf sqlite-amalgamation-*
	rm -rf *.zip
