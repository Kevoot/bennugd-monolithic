LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := SDL2_mixer

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../SDL/include \
	$(LOCAL_PATH)/../mikmod/include \
    $(LOCAL_PATH)/../libtremor \
    $(LOCAL_PATH)/../libtremor/ogg \

LOCAL_CFLAGS := -DWAV_MUSIC -DOGG_MUSIC -DOGG_USE_TREMOR -DMOD_MUSIC

LOCAL_SRC_FILES := $(notdir $(filter-out %/playmus.c %/playwave.c, $(wildcard $(LOCAL_PATH)/*.c)))

LOCAL_SHARED_LIBRARIES := SDL2 mikmod
LOCAL_STATIC_LIBRARIES := tremor

include $(BUILD_SHARED_LIBRARY)
