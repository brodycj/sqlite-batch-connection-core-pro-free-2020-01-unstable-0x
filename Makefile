include SQLite.mk

all:
	echo 'all target not supported'
	exit 1

sqlite-amalgamation:
	curl -O $(SQLITE_ZIP_URL)
	unzip $(SQLITE_ZIP)

distclean:
	make -C ctest distclean
	make -C sccglue distclean
	make -C cordova-demo distclean
	rm -rf sqlite-amalgamation-*
