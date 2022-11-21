LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := keyirqapp
LOCAL_SRC_FILES := keyirqapp.c
LOCAL_MODULE_PATH += $(LOCAL_PATH)
include $(BUILD_EXECUTABLE)
