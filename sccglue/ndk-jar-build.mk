# based on Makefile from:
# https://github.com/sqlg/SQLiteGlue-core

NDK_JAR_FILENAME := sqlite-connection-core-glue.jar

include ../sqlite-download.mk

include scclib.mk

ndkbuild: jar

jar: $(SQLITE_AMALGAMATION)/sqlite3.c
	ndk-build
	cp -r libs lib
	javac -d . java/io/sqlc/SCCoreGlue.java
	jar cvf $(NDK_JAR_FILENAME) lib io

ndkclean:
	rm -rf obj lib libs io *.jar

ndkdistclean: ndkclean
