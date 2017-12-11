/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "DCR.h"
#include "FaceFinder.h"

#define TAG "VideoPlayer"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG ,  TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO ,   TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN ,   TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR ,  TAG, __VA_ARGS__)
#define LOGXWL(x, ...) LOGV("%s(%d): "x, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define MAX_FILE_PATH 256

#define LOGFI LOGXWL

char g_algpath[MAX_FILE_PATH];
typedef struct
{
	char *buffer;
	off_t   size;
}AlgConf;
AlgConf g_algconf;

typedef struct
{
    float confidence;
    float midpointx;
    float midpointy;
    float eyedist;
}FaceData;
FaceData faceData;
//FaceOffsets
typedef struct
{
    jfieldID    confidence;
    jfieldID    midpointx;
    jfieldID    midpointy;
    jfieldID    eyedist;
    jfieldID    eulerx;
    jfieldID    eulery;
    jfieldID    eulerz;
} FaceOffsets;
//FaceOffsets gFaceOffsets;
typedef struct
{
	btk_HFaceFinder    fd;
	btk_HSDK    sdk;
	btk_HDCR    dcr;
	int    width;
	int    height;
	int    maxFaces;
} FaceDetectorOffsets;
FaceDetectorOffsets gFaceDetectorOffsets;









JNIEXPORT  void  Java_com_stereo_FaceDetector_FaceDetector_nativeSetEyeDistance(JNIEnv* env,jclass tis, jint minDist, jint maxDist)
{
	g_minDist = minDist;
	g_maxDist = maxDist;
	return ;
}

void  Java_com_stereo_FaceDetector_FaceDetector_nativeConfigAlg(JNIEnv* env,jclass tis,jobject assetManager,jstring filename)
//void  Java_com_example_hellojni_HelloJni_nativeConfigAlg(JNIEnv* env,jclass tis,jobject assetManager,jstring filename)
{
   LOGI("ReadAssets");
   AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
   if(mgr==NULL)
   {
      LOGI(" %s","AAssetManager==NULL");
      return ;
   }

   jboolean iscopy;
   const char *mfile = (*env)->GetStringUTFChars(env, filename, &iscopy);
   AAsset* asset = AAssetManager_open(mgr, mfile,AASSET_MODE_UNKNOWN);
   (*env)->ReleaseStringUTFChars(env, filename, mfile);
   if(asset==NULL)
   {
      LOGI(" %s","asset==NULL");
      return ;
   }

   g_algconf.size = AAsset_getLength(asset);
   LOGI("file size         : %d\n",g_algconf.size);
   g_algconf.buffer=(char *)malloc(g_algconf.size+1);
   g_algconf.buffer[g_algconf.size]=0;
   int numBytesRead = AAsset_read(asset, g_algconf.buffer, g_algconf.size);

   AAsset_close(asset);
}



jint  Java_com_stereo_FaceDetector_FaceDetector_nativeFaceInit( JNIEnv* env, jobject thiz,jint width, jint height)
//jint  Java_com_example_hellojni_HelloJni_nativeFace( JNIEnv* env, jobject _this, jstring pict_path, jint width, jint height)
{
	int ret = 0;
	int i;
	u32 maxFaces = 15;//_env->GetIntField(_this, gFaceDetectorOffsets.maxFaces);

	btk_HDCR dcr = NULL;
	btk_HSDK sdk = NULL;
	btk_HFaceFinder fd = NULL;

	btk_SDKCreateParam sdkParam = btk_SDK_defaultParam();
	sdkParam.fpMalloc = malloc;
	sdkParam.fpFree = free;
	sdkParam.maxImageWidth = width;
	sdkParam.maxImageHeight = height;

	btk_Status status = btk_SDK_create(&sdkParam, &sdk);
	// make sure everything went well
	if (status != btk_STATUS_OK) {
		// XXX: be more precise about what went wrong
		LOGI("btk_SDK_create error\n");
		return 0;
	}


	btk_DCRCreateParam dcrParam = btk_DCR_defaultParam();
	btk_DCR_create( sdk, &dcrParam, &dcr );


	btk_FaceFinderCreateParam fdParam = btk_FaceFinder_defaultParam();
	fdParam.pModuleParam = g_algconf.buffer;
	fdParam.moduleParamSize = g_algconf.size;
	fdParam.maxDetectableFaces = maxFaces;
	status = btk_FaceFinder_create( sdk, &fdParam, &fd );

	if(0==g_minDist || 0==g_maxDist)
		btk_FaceFinder_setRange(fd, 60, width/2); // set default eye distance range
	else
		btk_FaceFinder_setRange(fd, g_minDist, g_maxDist); // set eye distance range
	if (status != btk_STATUS_OK)
	{
		// XXX: be more precise about what went wrong
		//doThrow(_env, "java/lang/OutOfMemoryError", NULL);
		LOGI("btk_FaceFinder_setRange error\n");
		return 0;
	}

    gFaceDetectorOffsets.fd = fd;
    gFaceDetectorOffsets.sdk = sdk;
    gFaceDetectorOffsets.dcr = dcr;
    gFaceDetectorOffsets.width = width;
    gFaceDetectorOffsets.height = height;

    LOGXWL("resultion: %d %d",width,height);

	return 1;
}

//jint  Java_com_stereo_FaceDetector_FaceDetector_nativeDetect( JNIEnv* env, jobject thiz,jbyte* pBuffer)
jint  Java_com_stereo_FaceDetector_FaceDetector_nativeDetect( JNIEnv* env, jobject thiz,jbyteArray data)
{


	unsigned char *pBuffer = (*env)->GetByteArrayElements(env,data,NULL);

	if(pBuffer == NULL){
		LOGFI("GetByteArrayElements failed ");
		return 0;
	}

	btk_HDCR dcr = gFaceDetectorOffsets.dcr;
	btk_HFaceFinder fd = gFaceDetectorOffsets.fd;
	int width =  gFaceDetectorOffsets.width;
	int height = gFaceDetectorOffsets.height;
//	LOGXWL("resultion: %d %d",width,height);
	int numberOfFaces = 0;
	btk_DCR_assignGrayByteImage(dcr, pBuffer, width, height);

	if(pBuffer == NULL) LOGFI("pBuffer is NULL");

	FILE *fp = fopen("/sdcard/test/ss.txt","w+");
	if(fp != NULL){
		fwrite(pBuffer,57600,1,fp);
		fclose(fp);
	}

//	LOGXWL("resultion: %d %d",width,height);
	if (btk_FaceFinder_putDCR(fd, dcr) == btk_STATUS_OK)
	{
	//	LOGXWL();
		numberOfFaces = btk_FaceFinder_faces(fd);
	} else {
		LOGXWL("ERROR: Return 0 faces because error exists in btk_FaceFinder_putDCR.\n");
	}
//	LOGXWL("find face: %d",numberOfFaces);

	(*env)->ReleaseByteArrayElements(env,data,pBuffer,0);

	return numberOfFaces;
}

static void getFaceData(btk_HDCR hdcr, FaceData* fdata)
{
//	LOGXWL();
    btk_Node leftEye, rightEye;

    btk_DCR_getNode(hdcr, 0, &leftEye);
    btk_DCR_getNode(hdcr, 1, &rightEye);

    fdata->eyedist = (float)(rightEye.x - leftEye.x) / (1 << 16);
    fdata->midpointx = (float)(rightEye.x + leftEye.x) / (1 << 17);
    fdata->midpointy = (float)(rightEye.y + leftEye.y) / (1 << 17);
    fdata->confidence = (float)btk_DCR_confidence(hdcr) / (1 << 24);
//    LOGXWL("eyedist=%lf", fdata->eyedist);
}

void  Java_com_stereo_FaceDetector_FaceDetector_nativeGetFace(JNIEnv *env, jobject _this, jobject face, jint index)
//void  Java_com_example_hellojni_HelloJni_nativeGetFace(JNIEnv *env, jobject _this, jobject face, jint index)
{
//	LOGXWL();
	//btk_HDCR hdcr = (btk_HDCR)((*env)->GetIntField(env, _this, gFaceDetectorOffsets.dcr));
	//btk_HFaceFinder hfd = (btk_HFaceFinder)((*env)->GetIntField(env, _this, gFaceDetectorOffsets.fd));
	btk_HDCR hdcr = (btk_HDCR)gFaceDetectorOffsets.dcr;
	btk_HFaceFinder hfd = (btk_HFaceFinder)gFaceDetectorOffsets.fd;

	//FaceData faceData;
	btk_FaceFinder_getDCR(hfd, hdcr);
	getFaceData(hdcr, &faceData);
//	LOGXWL();
}

jfloat  Java_com_stereo_FaceDetector_FaceDetector_nativeGetConfidence(JNIEnv *env, jobject _this)
//jfloat  Java_com_example_hellojni_HelloJni_nativeGetConfidence(JNIEnv *env, jobject _this)
{
//	LOGFI();
	return (jfloat) faceData.confidence;
}
jfloat  Java_com_stereo_FaceDetector_FaceDetector_nativeGetMidpointx(JNIEnv *env, jobject _this)
//jfloat  Java_com_example_hellojni_HelloJni_nativeGetMidpointx(JNIEnv *env, jobject _this)
{
//	LOGFI();
	return (jfloat) faceData.midpointx;
}
jfloat  Java_com_stereo_FaceDetector_FaceDetector_nativeGetMidpointy(JNIEnv *env, jobject _this)
//jfloat  Java_com_example_hellojni_HelloJni_nativeGetMidpointy(JNIEnv *env, jobject _this)
{
//	LOGFI();
	return (jfloat) faceData.midpointy;
}
//jfloat  Java_com_example_hellojni_HelloJni_nativeGetEyedist(JNIEnv *env, jobject _this)
jfloat  Java_com_stereo_FaceDetector_FaceDetector_nativeGetEyedist(JNIEnv *env, jobject _this)
{
//	LOGFI();
	return (jfloat) faceData.eyedist;
}

//void  Java_com_cameraView_CameraView_nativeFaceRelease( JNIEnv* env, jobject thiz)
void  Java_com_stereo_FaceDetector_FaceDetector_nativeFaceRelease( JNIEnv* env, jobject thiz)
{
//	LOGFI();
	free(g_algconf.buffer);
	return ;
}

