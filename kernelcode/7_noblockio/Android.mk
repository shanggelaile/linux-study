LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := epoll
LOCAL_SRC_FILES := epoll.c
LOCAL_MODULE_PATH += $(LOCAL_PATH)
include $(BUILD_EXECUTABLE)
