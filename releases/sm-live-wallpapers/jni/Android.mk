LOCAL_PATH := $(call my-dir)

#lua############################

include $(CLEAR_VARS)

LOCAL_MODULE    := lua
FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../dependencies/includes/lua/*.c)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

include $(BUILD_STATIC_LIBRARY)

#libpng########################

include $(CLEAR_VARS)

LOCAL_CFLAGS := 

LOCAL_MODULE    := libpng
FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../dependencies/includes/libpng/*.c)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

include $(BUILD_STATIC_LIBRARY)

#ghost##########################

include $(CLEAR_VARS)

LOCAL_MODULE := ghost

FILE_LIST := $(wildcard $(LOCAL_PATH)/main_android.cpp)

FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../dependencies/includes/freetype/*.c)

FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../ghost/Multiplatform/Android/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../ghost/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../ghost/**/*.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
#LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -L$(SYSROOT)/usr/lib -ldl -L$(SYSROOT)/usr/lib -lz
LOCAL_LDLIBS := -llog -lGLESv2 -L$(SYSROOT)/usr/lib -ldl -L$(SYSROOT)/usr/lib -lz

# Support for additional libraries
#LOCAL_CFLAGS := -D__GXX_EXPERIMENTAL_CXX0X__ -std=gnu++11
#LOCAL_CFLAGS := -DANDROID_NDK -Wno-psabi -std=gnu++11
LOCAL_CFLAGS := -Os -fvisibility=hidden -ffunction-sections -fdata-sections -DDISABLE_PHYSICS -DDISABLE_SOUND
LOCAL_LDFLAGS := -Wl,--gc-sections
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libpng/ $(LOCAL_PATH)/lua/
LOCAL_STATIC_LIBRARIES := libpng lua

include $(BUILD_SHARED_LIBRARY)

#$(call import-module,android/native_app_glue)
