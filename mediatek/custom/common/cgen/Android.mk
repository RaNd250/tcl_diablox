LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)
#LOCAL_ARM_MODE:=arm
#Added [BEGIN] by jrd.lipeng for audio parameters customization(PR408943)
ifeq ($(JRD_AUDIO_PARAMS_CUST_M),yes)
LOCAL_CFLAGS += -DJRD_AUDIO_PARAMS_CUST
endif
#Added [END] by jrd.lipeng for audio parameters customization(PR408943)
# DMNR 3.0

# VOIP enhance Support
ifeq ($(MTK_VOIP_ENHANCEMENT_SUPPORT),yes)
LOCAL_CFLAGS += -DMTK_VOIP_ENHANCEMENT_SUPPORT
endif
# VOIP enhance Support

LOCAL_SHARED_LIBRARIES:= libc libcutils
LOCAL_SRC_FILES:= \
	CFG_file_info.c
LOCAL_C_INCLUDES:= \
      $(MTK_PATH_CUSTOM)/cgen/inc \
      $(MTK_PATH_CUSTOM)/cgen/cfgfileinc \
      $(MTK_PATH_CUSTOM)/cgen/cfgdefault \
      $(MTK_PATH_SOURCE)/external/nvram/libnvram
LOCAL_MODULE:=libcustom_nvram
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)


