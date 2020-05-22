include ../sqlite-download.mk

all:
	echo 'all target not supported' && exit 1

$(SQLITE_AMALGAMATION)/sqlite3.c:
	curl -O $(SQLITE_ZIP_URL)
	unzip $(SQLITE_ZIP)

prepare-plugin: $(SQLITE_AMALGAMATION)/sqlite3.c
	mkdir -p cordova-sqlite-demo-plugin/dependencies
	cp ../sccglue/sqlite-connection-core-glue.jar cordova-sqlite-demo-plugin/dependencies
	cp $(SQLITE_AMALGAMATION)/sqlite3.* cordova-sqlite-demo-plugin/dependencies
	cp ../sqlite-connection-core.[hc] cordova-sqlite-demo-plugin/dependencies
	cp ../android/src/main/java/io/sqlc/SQLiteBatchCore.java cordova-sqlite-demo-plugin/dependencies
	cp ../ios/SQLiteBatchCore.[hm] cordova-sqlite-demo-plugin/dependencies

prepare-app: prepare-plugin
	cordova plugin add ./cordova-sqlite-demo-plugin
	cordova plugin add cordova-sqlite-storage-file
	cordova plugin add cordova-plugin-file

clean:
	rm -rf node_modules package-lock.json platforms plugins
	rm -rf node_modules platforms plugins
	rm -rf cordova-sqlite-demo-plugin/dependencies

distclean: clean
	rm -rf sqlite-amalgamation-*
