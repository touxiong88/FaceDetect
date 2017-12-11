#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <pthread.h>
#include "profile.h"
#include "fflog.h"
#include "config.h"
#include "interLeaveBitmap.h"
#include "ffthread.h"


//double gWidth = gWidth (2.89f)//LeTV
//#define gWidth = (3.0);//T1 T1S
double gWidth = (3.044259);//xiaomi6
//double gWidth = (3.0780144f);//hauweiP10
static bool inited = 0;
static bool bitmapInited = 0;

static uint8_t *dataList[MAX_INTER_BITMAP] = {NULL};
static uint8_t *dataAll = NULL;

int gHoloWidth;
int gHoloHeight;
int gHoloSize;
int newd = 0;
bool reload = false;

static Mutex mLock;

void freeMemory();
int initMemory();

int holographyInit(int width, int height){

    LOGFI("width = %d   height = %d", width, height);

    Mutex::Autolock autoLock(mLock);
    
    gHoloWidth = width;
    gHoloHeight = height;
    gHoloSize = gHoloWidth * gHoloHeight;
	if(inited){
		LOGFI("has inited!!");
		return 0;
	}

 	int ret = initMemory();
	if(ret  == -1){
		return  -1;
	}
	ret = getBitmapList(dataList);
	LOGXWL("holographyInit ret = %d", ret);
	if(ret  == -1){
		LOGE("can not init bitmap");
		freeMemory();
		return  -1;
	}
	bitmapInited = 1;

	return 1;
    
}

void holographyDeinit(){
	
	Mutex::Autolock autoLock(mLock);
	LOGFI();

	if(bitmapInited){
		freeMemory();
	}

	bitmapInited = 0;
	inited = 0;

//	close(dev_fd);
	
	LOGFI();
}

int initMemory(){

	LOGFI();
	
	dataAll = (uint8_t*)malloc(gHoloSize*MAX_INTER_BITMAP);

	if(dataAll == NULL){
		LOGFI("not enough mem,%d",errno);
		return -1;
	}

	memset(dataList, 0, sizeof(uint8_t *) * MAX_INTER_BITMAP);
	memset(dataAll, 0, gHoloSize*MAX_INTER_BITMAP);

	int i;
	for(i = 0;i < MAX_INTER_BITMAP; i++){
		dataList[i] = dataAll + i * gHoloSize;
	}

	return 0;
}

void freeMemory(){

	LOGFI();
	int i;
	for(i = 0;i < MAX_INTER_BITMAP; i++){
		dataList[i] = NULL;
	}
	free(	dataAll );
	dataAll= NULL;
}


void updateImage(){

    
	if (!bitmapInited) {
		LOGXWL("bitmapInited is false");
		return;
	}


    Mutex::Autolock autoLock(mLock);

    if(reload){
    
        if( newd > (MAX_INTER_BITMAP - 1) ){
        	LOGXWL("invalue index!!!%d",newd);
        	newd = newd%(MAX_INTER_BITMAP - 1);
        }

        if(newd < 0){
        	LOGXWL("invalue index!!!%d",newd);
        	newd = (MAX_INTER_BITMAP - 1);
        }

        	LOGIT("newd:%d",newd);
        uint8_t *tmp = dataList[newd];
        glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
        glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, gHoloWidth, gHoloHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, tmp);
        reload = false;
    }
	
}


uint8_t *gblRaserData = NULL;
GLuint gblTextureId = 0;

int gblChangeRaser = 0;

int initRaserData(int screenWidth, int screenHeight)
{
    LOGFI("screensize(%d %d)", screenWidth, screenHeight);
    gHoloWidth = screenWidth;
    gHoloHeight = screenHeight;
    gHoloSize = gHoloWidth * gHoloHeight;

    gblRaserData = (uint8_t*)malloc(gHoloSize);

    glGenTextures(1, &gblTextureId);
    glBindTexture(GL_TEXTURE_2D, gblTextureId);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

int calRaserWidth(float angle, float raserWidth)
{
    if(gblChangeRaser == 1) {
        gAngle = angle;
        gWidth = raserWidth;

        LOGFI("gAngle = %f   gWidth = %f", gAngle, gWidth);

        initGlobalARG(angle);
        calImage(gblRaserData, 0);

        glBindTexture(GL_TEXTURE_2D, gblTextureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, gHoloWidth, gHoloHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, gblRaserData);

        gblChangeRaser = 0;
    }
	return gblTextureId;
}

void updateRaserTexture()
{
    gblChangeRaser = 1;
}


int setAngle(float angle){

	
	Mutex::Autolock autoLock(mLock);

	if(bitmapInited){
		bitmapInited = 0;
		interleaveDeinit(dataList);
	}



	int ret = initMemory();
	if(ret  == -1){
		LOGFI();
		return  -1;
	}
	LOGFI("%p  %p  %p",dataList,dataList[0],dataAll);
	ret = getBitmapListMultiThread(dataList,angle);
	if(ret  == -1){
		LOGE("can not init bitmap");
		return  -1;
	}

	inited = 1;
	bitmapInited = 1;
	return 0;
}


void sendDelt(int n){
	newd = n;
	reload = true;
}

void sendDeltFromAPP(int n){
	LOGFI("%d",n);
	newd = n;
	reload = true;
}
