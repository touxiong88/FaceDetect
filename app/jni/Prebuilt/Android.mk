LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := estar_vdpc_check
LOCAL_SRC_FILES := libestar_vdpc_check.so
include $(PREBUILT_SHARED_LIBRARY)

#############################################

include $(CLEAR_VARS)
LOCAL_MODULE := estar_v1
LOCAL_SRC_FILES := libestar_v1.so
include $(PREBUILT_SHARED_LIBRARY)

#############################################

include $(CLEAR_VARS)
LOCAL_MODULE := estar_v2
LOCAL_SRC_FILES := libestar_v2.so
include $(PREBUILT_SHARED_LIBRARY)

#############################################

include $(CLEAR_VARS)
LOCAL_MODULE := estar_v3
LOCAL_SRC_FILES := libestar_v3.so
include $(PREBUILT_SHARED_LIBRARY)

#############################################

include $(CLEAR_VARS)
LOCAL_MODULE := estar_check_picture
LOCAL_SRC_FILES := libestar_check_picture.so
include $(PREBUILT_SHARED_LIBRARY)

#############################################

include $(CLEAR_VARS)
LOCAL_MODULE := 3djni
LOCAL_SRC_FILES := lib3djni.so
include $(PREBUILT_SHARED_LIBRARY)

#############################################
