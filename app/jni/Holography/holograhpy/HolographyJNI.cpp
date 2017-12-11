#include <jni.h>
#include <stdio.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "fflog.h"
#include "FaceDetector.h"
#include "holography.h"

#include "profile.h"

extern "C"
{

HolographyProfile profile;


/*
 * Class:     com_stereo_Holography_Holography
 * Method:    HolographyInit
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT jint JNICALL Java_com_stereo_Holography_Holography_HolographyInit
  (JNIEnv *env, jclass jc,int width,int height){
	LOGFI("%d %d",width,height);
	int ret = readProfileFile(&profile);
	if (ret < 0) {
		LOGXWL("readProfileFile error");
	}
	baseView = profile.baseView;
	gAngle = profile.angle;
	gWidth = profile.gwidth;
	LOGIT("baseView = %d", baseView);
	LOGIT("gAngle = %f", gAngle);
	LOGIT("gWidth = %f", gWidth);
    return holographyInit(width,height);
}

JNIEXPORT void JNICALL Java_com_stereo_Holography_Holography_deinitHolography
  (JNIEnv *env, jclass jc){
        LOGFI();
		if (profile.data != NULL) {
			free(profile.data);
			profile.data = NULL;
			LOGFI("free profile.data");
		}
        holographyDeinit();
}

JNIEXPORT void JNICALL Java_com_stereo_Holography_Holography_Release
  (JNIEnv *env, jclass obj){
	LOGFI();
	holographyDeinit();
}

JNIEXPORT void JNICALL Java_com_stereo_Holography_Holography_update
   (JNIEnv *env, jclass cls){
	
	updateImage();
}

JNIEXPORT void JNICALL Java_com_stereo_Holography_Holography_setAngle
  (JNIEnv *env, jclass cls,jfloat a){
	profile.angle = a;
	setAngle(a);
}
JNIEXPORT void JNICALL Java_com_stereo_Holography_Holography_sendDelt
  (JNIEnv *env, jclass obj,jint n){
	LOGXWL();
	sendDeltFromAPP(n);
}


int JNICALL Java_com_stereo_Holography_Holography_getx
  (JNIEnv *env, jclass cls){
	return gX;
}

int JNICALL Java_com_stereo_Holography_Holography_gety
  (JNIEnv *env, jclass cls){
	return gY;
}

int JNICALL Java_com_stereo_Holography_Holography_getdis
  (JNIEnv *env, jclass cls){
	return gEyeDis;
}


JNIEXPORT jint JNICALL Java_com_stereo_Holography_Holography_initRaserData
(JNIEnv *env, jclass cls, jint screenWidth, jint screenHeight){
    return initRaserData(screenWidth, screenHeight);
}

JNIEXPORT jint JNICALL Java_com_stereo_Holography_Holography_calRaserWidth
(JNIEnv *env, jclass cls, jfloat angle, jfloat raserWidth){

	return calRaserWidth(angle, raserWidth);
}

JNIEXPORT void JNICALL Java_com_stereo_Holography_Holography_updateRaserTexture
(JNIEnv *env, jclass cls){
    updateRaserTexture();
}



int JNICALL Java_com_stereo_Holography_Holography_getBaseView
(JNIEnv *env, jclass cls){
	return baseView;

}

jfloat JNICALL Java_com_stereo_Holography_Holography_getAngle
(JNIEnv *env, jclass cls){
	return gAngle;

}

void JNICALL Java_com_stereo_Holography_Holography_setBaseView
(JNIEnv *env, jclass cls, jint basev){
	profile.baseView = basev;
	baseView = basev;
	LOGFI("baseView %d", baseView);
}


float JNICALL Java_com_stereo_Holography_Holography_getGWidth
(JNIEnv *env, jclass cls){
	return gWidth;

}

void JNICALL Java_com_stereo_Holography_Holography_setGWidth
(JNIEnv *env, jclass cls, float rasWidth){
	gWidth = rasWidth;
	LOGFI("gWidth = %f", gWidth);

}

void JNICALL Java_com_stereo_Holography_Holography_save
(JNIEnv *env, jclass cls){
    LOGFI("baseView = %d", baseView);
	saveProfileFile(gAngle, gWidth, profile.cwidth, profile.cheight, profile.baseY, baseView, profile.data, profile.num);
}





///////////////////////////////////////////////////////

JNIEXPORT  jint  Java_com_stereo_FaceDetector_FaceDetector_nativeFaceDetectInit
(JNIEnv* env,jclass tis,jint width, jint height,jobject assetManager,jstring filename ,jint minDist, jint maxDist){

    LOGFI("ReadAssets");
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    if(mgr==NULL)
    {
      LOGI(" %s","AAssetManager==NULL");
      return -1;
    }

    jboolean iscopy;
    const char *mfile = env->GetStringUTFChars(filename, &iscopy);
    AAsset* asset = AAssetManager_open(mgr, mfile,AASSET_MODE_UNKNOWN);
    env->ReleaseStringUTFChars(filename, mfile);
    if(asset==NULL)
    {
      LOGFI(" %s","asset==NULL");
      return -2;
    }

    g_algconf.size = AAsset_getLength(asset);
    LOGFI("file size         : %ld\n",g_algconf.size);
    g_algconf.buffer=(char *)malloc(g_algconf.size+1);
    g_algconf.buffer[g_algconf.size]=0;
    int numBytesRead = AAsset_read(asset, g_algconf.buffer, g_algconf.size);

    AAsset_close(asset);

    int re = FaceDetectInit(width,height,minDist,maxDist);
    
    return re;
}


jint  Java_com_stereo_FaceDetector_FaceDetector_nativeDetect( JNIEnv* env, jobject thiz,jbyteArray data)
{


	unsigned char *pBuffer = (unsigned char*)(env->GetByteArrayElements(data,0));

	if(pBuffer == NULL){
		LOGFI("GetByteArrayElements failed ");
		return 0;
	}

       int ret = FaceDetect(pBuffer);

       env->ReleaseByteArrayElements(data,(jbyte*)pBuffer,0);

    
	return ret;
}

void  Java_com_stereo_FaceDetector_FaceDetector_nativeRelease( JNIEnv* env, jobject thiz )
{

    
    FaceDetectRelease();

}


JNIEXPORT float  Java_com_stereo_FaceDetector_FaceDetector_GetEyeLeftX(JNIEnv *env, jobject _this)
{
	return (jfloat)  leftEye.x/(1 << 16);
}
JNIEXPORT float  Java_com_stereo_FaceDetector_FaceDetector_GetEyeRightX(JNIEnv *env, jobject _this)
{
	return (jfloat) rightEye.x/(1 << 16);
}
JNIEXPORT float  Java_com_stereo_FaceDetector_FaceDetector_GetEyeLeftY(JNIEnv *env, jobject _this)
{
	return (jfloat) leftEye.y/(1 << 16);
}
JNIEXPORT float  Java_com_stereo_FaceDetector_FaceDetector_GetEyeRightY(JNIEnv *env, jobject _this)
{
	return (jfloat) rightEye.y/(1 << 16);
}


}// end extern c
