#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "DCR.h"
#include "FaceFinder.h"

#include "fflog.h"
#include "config.h"

extern int baseView;
extern btk_Node leftEye, rightEye;
struct AlgConf
{
	char *buffer;
	off_t   size;
};

struct FaceDetectorOffsets
{
	btk_HFaceFinder    fd;
	btk_HSDK    sdk;
	btk_HDCR    dcr;
	int    width;
	int    height;
	int    maxFaces;
};
struct FaceData
{
    float confidence;
    float midpointx;
    float midpointy;
    float eyedist;
};

#define N 8

struct QFaceData {
	float x;        // x 坐标
	float y;	// y 坐标
	float speed;    // 速度
	float ctime;    // 时间 
	float eyedis;   // 两眼的间距
	int dir;	// 方向
};

struct QueueFace {
	QFaceData facedata[N];
	int head;
	int tail;
	float totalX;
	float totalY;
	float totalEyeDis;
	int state[3];   // 2 速度为正的个数， 0 速度为负的个数， 1 速度为 0 的个数
};

extern AlgConf g_algconf;
extern int gX, gY, gEyeDis;

void FaceDetectRelease();
int FaceDetectInit(int width,int height, int edis_min,int edis_max);
int FaceDetect(unsigned char *pBuffer);

void sendPosX(int pos,int y, int eyedis);
int pushSpeed(float speed, float x, float y, float eyedis, long ctime, int dir);
int calPos(float speed, float x, float y, float eyedis, long ctime, int dir);
int sizeQueue();
void calArea(int x,int y);