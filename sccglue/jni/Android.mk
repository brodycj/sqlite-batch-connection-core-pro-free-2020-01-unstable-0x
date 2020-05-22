# THANKS for initial guidance:
# * https://code.tutsplus.com/tutorials/advanced-android-getting-started-with-the-ndk--mobile-2152
# redirected from: http://mobile.tutsplus.com/tutorials/android/ndk-tutorial/

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../sqlite-download.mk

include $(LOCAL_PATH)/../../sqlite-build.mk

# FUTURE TBD (??):
# LOCAL_LDLIBS := -llog

LOCAL_MODULE := sqlc-connection-core-glue

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../$(SQLITE_AMALGAMATION)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../..

LOCAL_CFLAGS += $(SQLITE_CFLAGS)

LOCAL_SRC_FILES := sccglue_jni_all.c

include $(BUILD_SHARED_LIBRARY)

