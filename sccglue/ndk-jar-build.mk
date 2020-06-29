# based on Makefile from:
# https://github.com/sqlg/SQLiteGlue-core

NDK_JAR_FILENAME := sqlite-connection-core-glue.jar

include ../sqlite-download.mk

include scclib.mk

include java-build.mk

ndkbuild: jar

jar: $(SQLITE_AMALGAMATION)/sqlite3.c javac
	rm -rf lib libs
	ndk-build
	cp -r libs lib
	jar cvf $(NDK_JAR_FILENAME) lib io

ndkclean:
	rm -rf obj lib libs io *.jar

ndkdistclean: ndkclean
