LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/libs.mk

include $(CLEAR_VARS)

$(shell touch -m $(LOCAL_PATH)/sys.cpp)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := desktop

DIRS = . ui logger misc app d600 mqtt \
	app/intercom app/setup app/message app/intell_name app/intell_camera\
	app/security app/smart app/misc app/intell_set app/intell_number app/intell_history\
	nTouch nTouch/intercom nTouch/elev \
	nTouch/msg nTouch/setup nTouch/security

INC  = $(foreach dir,$(DIRS),$(LOCAL_PATH)/$(dir))
SRC  = $(foreach dir,$(DIRS),$(wildcard $(LOCAL_PATH)/$(dir)/*.c))
SRC += $(foreach dir,$(DIRS),$(wildcard $(LOCAL_PATH)/$(dir)/*.cpp))

LOCAL_SRC_FILES := $(SRC:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(INC) \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/freetype2 \
	$(LOCAL_PATH)/include/ipwatchd \
	$(LOCAL_PATH)/include/eclipse-paho-mqtt-c-src 	

LOCAL_LDFLAGS := -pie -fPIE -lz -lm
LOCAL_CFLAGS := -Wall -DLOG_TAG=\"desktop\"
LOCAL_CFLAGS += -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-unused-variable -Wno-array-bounds

#LOCAL_CFLAGS += -DD_ELEV

LOCAL_STATIC_LIBRARIES := libmisc libtinyxml liblog libjpeg \
	libpng libfreetype2 libavcodec libswscale libavutil \
	libiconv libpcap libnet libpaho-mqtt3a.a

LOCAL_CFLAGS += -mfloat-abi=softfp -mfpu=neon -march=armv7-a
LOCAL_ARM_NEON := true
TARGET_ARCH_ABI := armeabi-v7a
LOCAL_ARM_MODE := arm

include $(BUILD_EXECUTABLE)
