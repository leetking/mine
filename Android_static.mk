LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_SRC_FILES := src/main.c src/model.c src/timer.c

LOCAL_STATIC_LIBRARIES := SDL2_static SDL2_image_static

include $(BUILD_SHARED_LIBRARY)
$(call import-module,SDL)LOCAL_PATH := $(call my-dir)
