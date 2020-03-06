# THANKS for initial guidance:
# * https://code.tutsplus.com/tutorials/advanced-android-getting-started-with-the-ndk--mobile-2152
# redirected from: http://mobile.tutsplus.com/tutorials/android/ndk-tutorial/

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# FUTURE TBD (??):
# LOCAL_LDLIBS := -llog

LOCAL_MODULE := sqlite-connection-core-glue

include $(LOCAL_PATH)/../../SQLite.mk
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../$(SQLITE_AMALGAMATION)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../..

LOCAL_CFLAGS += \
	-DSQLITE_TEMP_STORE=2 -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS -DSQLITE_ENABLE_FTS4 -DSQLITE_ENABLE_RTREE

LOCAL_SRC_FILES := sccglue_jni_all.c

include $(BUILD_SHARED_LIBRARY)

