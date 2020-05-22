
include ../sqLite-download.mk

include ../sqLite-build.mk

include scclib.mk

JNI_C_SOURCE := jni/sccglue_jni_all.c

# JNI lib file for testing on macOS:
JNI_LIB_FILE := libsqlc-connection-core-glue.jnilib

GET_MAC_JAVA_HOME := /usr/libexec/java_home

jnilib: $(SQLITE_AMALGAMATION)/sqlite3.c
	# jnilib for testing on macOS only
	# THANKS FOR GUIDANCE:
	# https://gist.github.com/DmitrySoshnikov/8b1599a5197b5469c8cc07025f600fdb
	echo Java home is here: `$(GET_MAC_JAVA_HOME)`
	cc $(JNI_C_SOURCE) $(SQLITE_CFLAGS) -o $(JNI_LIB_FILE) -shared -I`$(GET_MAC_JAVA_HOME)`/include -I`$(GET_MAC_JAVA_HOME)`/include/darwin -I$(SQLITE_AMALGAMATION) -I$(SCCLIB_SOURCE_DIR)

$(JNI_LIB_FILE): jnilib

jnitestclean:
	rm -rf test*.db

jnitest: $(JNI_LIB_FILE) jnitestclean
	javac -sourcepath java java/SCCoreGlueTest.java
	java -cp java SCCoreGlueTest

jniclean: jnitestclean
	rm -rf java/*.class
	rm -rf java/io/sqlc/*.class
	rm -rf *.jnilib
