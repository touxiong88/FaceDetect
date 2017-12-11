#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>



#include "DCR.h"
#include "FaceFinder.h"


#include "FaceDetector.h"
#include "profile.h"

extern "C"
{
    #include <fd_emb_sdk.h>
}

int g_minDist, g_maxDist;

#define MAX_FILE_PATH 256

char g_algpath[MAX_FILE_PATH];

AlgConf g_algconf;
FaceDetectorOffsets gFaceDetectorOffsets;
FaceData faceData;
QueueFace queueFace;

int prevPos = 0;
long prevTime = 0;
int maxLen = 100;
int gX, gY, gEyeDis;
int preX, preY, preEyeDis;
int gdirMinCha = 2;
uint16_t ArrSize = 0;
int quanxiX = 0;
int baseView;
int baseY = 268;
static float int_x = 0.32;
float cspeed = 0;
int lastArea = 0;
int forward = 1;

float *AreaPoint;
float *AreaPoint2;


int frames = 0;
int faces = 0;
long pre_time = 0;
btk_Node leftEye, rightEye;
extern int newd;

static int  have_Profile = 0;

void sendDelt(int n);

void FaceDetectRelease(){

    LOGFI();
    btk_FaceFinder_close( gFaceDetectorOffsets.fd );
    btk_DCR_close(  gFaceDetectorOffsets.dcr );
    btk_SDK_close(  gFaceDetectorOffsets.sdk );

    free(g_algconf.buffer);
}


int FaceDetectInit(int width,int height, int edis_min,int edis_max){

    g_minDist = edis_min;
    g_maxDist = edis_max;
    

  	int ret = 0;
	int i;
	u32 maxFaces = 3;

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
		LOGFI("btk_SDK_create error\n");
		return -3;
	}


	btk_DCRCreateParam dcrParam = btk_DCR_defaultParam();
	btk_DCR_create( sdk, &dcrParam, &dcr );


	btk_FaceFinderCreateParam fdParam = btk_FaceFinder_defaultParam();
	fdParam.pModuleParam = g_algconf.buffer;
	fdParam.moduleParamSize = g_algconf.size;
	fdParam.maxDetectableFaces = maxFaces;
	status = btk_FaceFinder_create( sdk, &fdParam, &fd );

	if(0 == g_minDist || 0 == g_maxDist)
		btk_FaceFinder_setRange(fd, 60, width/2); // set default eye distance range
	else
		btk_FaceFinder_setRange(fd, g_minDist, g_maxDist); // set eye distance range
	if (status != btk_STATUS_OK)
	{
		// XXX: be more precise about what went wrong
		//doThrow(_env, "java/lang/OutOfMemoryError", NULL);
		LOGFI("btk_FaceFinder_setRange error\n");
		return -2;
	}

    gFaceDetectorOffsets.fd = fd;
    gFaceDetectorOffsets.sdk = sdk;
    gFaceDetectorOffsets.dcr = dcr;
    gFaceDetectorOffsets.width = width;
    gFaceDetectorOffsets.height = height;

    LOGFI("resultion: %d %d",width,height);  

    pre_time = getTimeStamp();


	HolographyProfile *profile = (HolographyProfile*)malloc(sizeof(HolographyProfile));


	ret =  readProfileFile(profile);

	if(ret  !=  0){
		LOGFI("profile is NULL ,no holography!!!");
		LOGFI("close face detector");
		free(profile);
             have_Profile = 0;
		return -1;
	}

	baseY = profile->baseY;
	baseView = profile->baseView;
	ArrSize = profile->num;
	
	int size = ArrSize*sizeof(uint32_t);
	AreaPoint = (float*)malloc(size);
	AreaPoint2 = (float*)malloc(size);

	memcpy(AreaPoint2,profile->data,size);

	free(profile->data);
	free(profile);
	profile = NULL;

	LOGFI("baseY:%d,baseView:%d,%d",baseY,baseView,ArrSize);
	for(i=0;i<ArrSize;i++){
		AreaPoint[i] = AreaPoint2[ArrSize-1-i];
//		LOGFI("%d,%d ",i,AreaPoint[i]);
	}
    have_Profile = 1;
    return 0;
}




int FaceDetect(unsigned char *pBuffer){
    frames++;
    if(have_Profile == 0 ){

        if( frames%100 == 0){
                LOGFI("no holo  profile  close  face detect!!!");
        }
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


//	LOGXWL("resultion: %d %d",width,height);
	if (btk_FaceFinder_putDCR(fd, dcr) == btk_STATUS_OK)
	{
	//	LOGXWL();
		numberOfFaces = btk_FaceFinder_faces(fd);
	} else {
		LOGXWL("ERROR: Return 0 faces because error exists in btk_FaceFinder_putDCR.\n");
	}


 //   LOGXWL("find face: %d",numberOfFaces);

    
    if(numberOfFaces > 0)  faces++;
    
    if(frames  >= 30){
        long ctime = getTimeStamp() ;
        if(ctime - pre_time > 300){
            LOGFI("face fps: %3.1f",(double)faces*1000 /(ctime - pre_time));
            faces = 0;
            frames = 0;
            pre_time = getTimeStamp();

        }
    }

	if (numberOfFaces <= 0)  {
//		LOGXWL("numberOfFaces = %d", numberOfFaces);
		return numberOfFaces;
	}
    
   // LOGXWL("find face: %d",numberOfFaces);

    btk_HDCR hdcr = (btk_HDCR)gFaceDetectorOffsets.dcr;
    btk_HFaceFinder hfd = (btk_HFaceFinder)gFaceDetectorOffsets.fd;

    //FaceData faceData;
    btk_FaceFinder_getDCR(hfd, hdcr);

    
    //btk_Node leftEye, rightEye;

    btk_DCR_getNode(hdcr, 0, &leftEye);
    btk_DCR_getNode(hdcr, 1, &rightEye);

    faceData.eyedist = (float)(rightEye.x - leftEye.x) / (1 << 16);
    faceData.midpointx =width -  (float)(rightEye.x + leftEye.x) / (1 << 17);
 //   faceData.midpointx = (float)(rightEye.x + leftEye.x) / (1 << 17);
    faceData.midpointy = (float)(rightEye.y + leftEye.y) / (1 << 17);
    faceData.confidence = (float)btk_DCR_confidence(hdcr) / (1 << 24);

    sendPosX( faceData.midpointx, faceData.midpointy, faceData.eyedist);
    
    LOGFI("%3.2f %3.2f %3.2f", faceData.midpointx,faceData.midpointy,faceData.eyedist);


    return numberOfFaces;
}



void cleanQueue(){

	LOGFI("11111111");
	memset(&queueFace,0,sizeof(QueueFace));

}

void sendPosX(int pos,int y, int eyedis){
	gX = 0;
	gY = 0;
	gEyeDis = 0;

	long ctime = getTimeStamp();
	int size = 1;
	if(prevPos != 0){
		float speed = (float)(pos - prevPos)/(ctime-prevTime);
		
		int dir = 0;
		if( speed > 0 ) {
			dir = 1;
		}
		else if( speed < 0 ) {
			dir = -1;
		}

	//	readParm2();
		if(pos - prevPos > maxLen){
			cleanQueue();
		}

		pushSpeed(speed, pos, y, eyedis, ctime, dir);
		calPos(speed, pos, y, eyedis, ctime, dir);
		calArea(gX, gY);	
	}

	prevPos = pos;
	prevTime = ctime;

}


int pushSpeed(float speed, float x, float y, float eyedis, long ctime, int dir){
	queueFace.facedata[queueFace.head].x = x;
	queueFace.facedata[queueFace.head].y = y;
	queueFace.facedata[queueFace.head].eyedis = eyedis;
	queueFace.facedata[queueFace.head].ctime = ctime;
	queueFace.facedata[queueFace.head].speed = speed;
	queueFace.facedata[queueFace.head].dir = dir;
	queueFace.state[dir+1]++;
	queueFace.totalX += x;
	queueFace.totalY += y;
	queueFace.totalEyeDis += eyedis;
	
	int pos = (queueFace.head + 1) % N;
	
	if( pos == queueFace.tail ) {
		int dir = queueFace.facedata[queueFace.tail].dir + 1;
		queueFace.totalX -= queueFace.facedata[queueFace.tail].x;
		queueFace.totalY -= queueFace.facedata[queueFace.tail].y;
		queueFace.totalEyeDis -= queueFace.facedata[queueFace.tail].eyedis;
		queueFace.state[dir]--;
		queueFace.tail = (queueFace.tail + 1) % N;
	}
	queueFace.head = pos;
	return 0;
}


int calPos(float speed, float x, float y, float eyedis, long ctime, int dir) {
	int head = (queueFace.head - 1 + N) % N;

	int size = sizeQueue();
		

	if( queueFace.state[dir+1] < gdirMinCha ) {
		gX = preX;
		gY = preY;
		gEyeDis = preEyeDis;
	}
	else if( queueFace.state[dir+1] >= size ) { 
		gX = x;
		gY = y;
		gEyeDis = eyedis;
	}
	else {
		gX = queueFace.totalX / size;
		gY = queueFace.totalY / size;
		gEyeDis = queueFace.totalEyeDis / size;
	}
	preX = gX;
	preY = gY;
	preEyeDis = gEyeDis;
	
	return 0;
}


int sizeQueue(){
	int head = (queueFace.head > queueFace.tail) ? queueFace.head : (queueFace.head + N);
	return head - queueFace.tail;
}


int findAreaR(int x, float data[]){
    int mid;
    int beg = 0;
    int last  = ArrSize-1;
    if (beg > last){
        return -1;
    }
    while(beg <= last){
        mid = (beg + last) / 2;
        if (x >= data[mid]  && x <= data[mid+1]){
            return mid;
        }else if (data[mid+1] < x){
            beg = mid + 1;
        }else if(data[mid] > x){
            last = mid - 1;
        }
    }
    return -1;
}

int findAreaL(int x, float data[]){
//	LOGXWL();
    int mid;
    int beg = 0;
    int last  = ArrSize-1;
    if (beg > last){
        return -1;
    }
    while(beg <= last){
        mid = (beg + last) / 2;
//	LOGFI("beg:%d,last:%d,mid:%d,%d %d\n",beg,last,mid,data[mid],data[mid+1]);
        if (x <= data[mid]  && x >= data[mid+1]){
//	    LOGFI("mid:%d",mid+1);
            return mid+1;
        }else if (data[mid+1] > x){
            beg = mid + 1;
        }else if(data[mid] < x){
            last = mid - 1;
        }
    }
    return -1;
}

void calArea(int x,int y){
	quanxiX = x;
	
//	/*
	int i = lastArea;
//	LOGXWL();
	int pos = x - (y - baseY)*int_x;

	if(pos <= AreaPoint[0]){
		lastArea = 0;
	}else if(pos >= AreaPoint[ArrSize-1]){
	//	LOGXWL("pos %d, %d",pos,AreaPoint[ArrSize-1]);
		lastArea = ArrSize % MAX_INTER_BITMAP;
	}else{


	
		if(forward > 0){
			i = findAreaR(pos,AreaPoint);
		//	LOGIT("i--R:%d",i%MAX_INTER_BITMAP);
			lastArea = i%MAX_INTER_BITMAP;
			if(cspeed > 0.1){
				lastArea = (lastArea+2)%MAX_INTER_BITMAP;
			}
		}else{
	//		int tmp = findAreaR(pos,AreaPoint);
			
			i = findAreaL(pos,AreaPoint2);
			lastArea = (ArrSize-i-1)%MAX_INTER_BITMAP;
			
	//		LOGIT("i--L:%d",lastArea);
	//		LOGFI("i--R:%d",tmp%MAX_INTER_BITMAP);
			
			if(cspeed < -0.1){
				lastArea = (lastArea-2)%MAX_INTER_BITMAP;
			}
			
		}
	}

	int tmparea = lastArea;
	if(pos > 450){
		//lastArea = lastArea + 32 - 10;
	}
	
	lastArea = (lastArea + baseView + 16)%32;

       sendDelt(lastArea);
     //  LOGIT("lastArea = %d", lastArea);
    //   newd = lastArea;
      
	//LOGIT("pos:%d,tmp:%d,lastArea:%d",pos,tmparea,lastArea);
	

//  */

}
