# based on Makefile from:
# https://github.com/sqlg/SQLiteGlue-core

GLUEGENTOOLS_PATH := gluegentools-master
GLUEGEN_CLASSPATH := $(GLUEGENTOOLS_PATH)/antlr.jar:$(GLUEGENTOOLS_PATH)/gluegen.jar
GLUEGEN_CLASS := com.jogamp.gluegen.GlueGen
GLUEGEN_INCLUDES := -I. -Ecom.jogamp.gluegen.JavaEmitter
GLUEGEN_CONFIG := -CSCCoreGlue.cfg
GLUEGEN_HEADER_FILE := $(SCCLIB_HEADER)

GLUEGEN_ARGS := $(GLUEGEN_INCLUDES) $(GLUEGEN_CONFIG) $(GLUEGEN_HEADER_FILE)

GLUEGEN_FIX_JAVA_FILE := java/io/sqlc/SCCoreGlue.java
GLUEGEN_FIX_JAVA_COMMAND := \
	sed -i.orig 's/^import/\/\/import/' $(GLUEGEN_FIX_JAVA_FILE)
GLUEGEN_REMOVE_JAVA_ORIG := rm -rf $(GLUEGEN_FIX_JAVA_FILE).orig

GLUEGEN_FIX_JNI_FILE := jni/SCCoreGlue_JNI.c
GLUEGEN_FIX_JNI_COMMAND := \
	sed -i.orig "s/  *$$//g" $(GLUEGEN_FIX_JNI_FILE)
GLUEGEN_REMOVE_JNI_ORIG := rm -rf $(GLUEGEN_FIX_JNI_FILE).orig

$(GLUEGENTOOLS_PATH):
	curl -OL https://github.com/sqlg/gluegentools/archive/master.zip
	unzip master.zip

regen: $(GLUEGENTOOLS_PATH)
	java -cp $(GLUEGEN_CLASSPATH) $(GLUEGEN_CLASS) $(GLUEGEN_PATH) $(GLUEGEN_ARGS)
	$(GLUEGEN_FIX_JAVA_COMMAND)
	$(GLUEGEN_REMOVE_JAVA_ORIG)
	$(GLUEGEN_FIX_JNI_COMMAND)
	$(GLUEGEN_REMOVE_JNI_ORIG)

glue-regen-distclean:
	rm -rf gluegentools
	rm -rf gluegentools-*
