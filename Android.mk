LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

DVERSION := 0.0.1

LOCAL_MODULE := main

APP_SOURCES := src/main.c src/model.c src/timer.c

SDL_PATH := ../SDL2
SDL_iamge_PATH = ../SDL2_image
SDL_ttf_PATH = ../SDL2_ttf

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
	$(LOCAL_PATH)/$(SDL_iamge_PATH) \
	$(LOCAL_PATH)/$(SDL_ttf_PATH)

LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c $(APP_SOURCES)

# SDL2_ttf、SDL2_image的顺序很重要
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_image

LOCAL_CFLAGS += -DVERSION=\"$(DVERSION)\"

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
