LOCAL_PATH:= $(call my-dir)

GTISDKROOT ?= ../../..

include $(CLEAR_VARS)
LOCAL_MODULE := libgti_android
LOCAL_SRC_FILES := $(GTISDKROOT)/Lib/Android/$(TARGET_ARCH_ABI)/libgti_android.so
include $(PREBUILT_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := libftd3xx
#LOCAL_SRC_FILES := $(GTISDKROOT)/Lib/Android/$(TARGET_ARCH_ABI)/libftd3xx.so
#include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libusb1.0
LOCAL_SRC_FILES := $(GTISDKROOT)/Lib/Android/$(TARGET_ARCH_ABI)/libusb1.0.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := ../liteDemo.cpp

LOCAL_MODULE:= liteDemo
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := libgti_android libusb1.0

include $(BUILD_EXECUTABLE) 

