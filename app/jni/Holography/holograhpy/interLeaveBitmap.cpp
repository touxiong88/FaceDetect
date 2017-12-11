#include <string.h>
#include <math.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>


#include "fflog.h"
#include "interLeaveBitmap.h"
#include "config.h"
#include "profile.h"




#define NUMTHREAD  4

typedef struct  INTERDATA{
	int  start;
	int  end;
	uint8_t **dataList;
}INTERDATA;

static pthread_t gThread[NUMTHREAD];

double gs1 = 0.2;;
double gs2 = 1.4;
double gs3 = 1.8;
double gs4 = 3.0;

//double delt_p = 0.00025;
double delt_p = 0.00015;
double inc_x;
double int_x;
double per_inc_x;
double per_delt;

static int pre_s[4] = {27,133,187,293};
static int carea = 30;

double getCoc(double dis_test,double int_x_v){

	double tmp = 0.5;

    if(dis_test <= gs1+int_x_v){
    	tmp = (1-(dis_test+inc_x-gs4)/(inc_x+gs1-gs4));
    }else if(dis_test <= gs2+int_x_v){
    	tmp = 0.0;
    }else if(dis_test <= gs3+int_x_v){
    	tmp = (dis_test - gs2)/(gs3-gs2);
    }else if(dis_test <= gs4+int_x_v){
    	tmp = 1.0;
    }else{
    	tmp = (1-(dis_test-gs4)/(inc_x+gs1-gs4));
    }

    return tmp;
}


void calImage(uint8_t *data, double delt_s){
	LOGFI("gAngle: %3.3f  delt:%f,gs:%f,%f,%f,%f",gAngle,delt_s,gs1,gs2,gs3,gs4);
	//	LOGFI("data:%p",data);
	int i,j;
	double y_pos = 0;
	double delt = 0;
	double dis_test = 0;
	uint8_t cob;
	int pos = 0;
	int pos1 = 0;
	double tmp = 0.0;
	LOGFI("calImage prepare gen data");
	LOGFI("gHoloWidth = %d  gHoloHeight = %d", gHoloWidth, gHoloHeight);
	for (i=0;i<gHoloHeight;i++){
		y_pos = i*int_x+delt_s;
		delt = fmod(y_pos,inc_x);
		pos1 = i*gHoloWidth;
		for(j=0;j<gHoloWidth;j++){
		        if(j > delt){
		            dis_test = fmod((inc_x+j - delt+j*delt_p),inc_x);
		        }else{
		            dis_test = delt - j;
		        }
		        pos = pos1 + (gHoloWidth - 1 - j);
		//        pos = pos1 + j;
		        tmp = getCoc(dis_test,0);
		        cob = tmp *255;
		        data[pos] = cob;
		}
	}
	LOGFI("calImage success");
}

//初始化参数
void initGlobalARG(double angle){

    LOGFI("angle : %f,gwidth %f",angle,gWidth);

	inc_x =  gWidth / cos(angle *M_PI / 180);
	per_inc_x = inc_x/320.0;

	int_x =  tan(angle * M_PI / 180);

	pre_s[0] = carea;
	pre_s[1] = 160-carea;
	pre_s[2] = 160+carea;
	pre_s[3] =  320 -carea;

	gs1 = (double)pre_s[0]*per_inc_x;
	gs2 = (double)pre_s[1]*per_inc_x;
	gs3 = (double)pre_s[2]*per_inc_x;
	gs4 = (double)pre_s[3]*per_inc_x;

	per_delt = inc_x/MAX_INTER_BITMAP;

	LOGFI("%d %d",gHoloWidth,gHoloHeight);
	LOGFI("%d,gs:%f,%f,%f,%f,%.16lf,per_delt:%f",carea,gs1,gs2,gs3,gs4,delt_p,per_delt);

}

void saveBitmap(double angle){


	initGlobalARG(angle);


	int fd;
	int size = gHoloSize;

	int ret = system("mkdir  /sdcard/Android");

	LOGFI("ret:%d",ret);

	fd = open(MATRIX_PATH,O_RDWR | O_CREAT,0777);
	if(fd < 0){
		LOGE("open %s err %d",MATRIX_PATH,errno);
		return;
	}

	int i;
	int total = 0;
	uint8_t *data = (uint8_t*)malloc(gHoloSize);
	for(i=0; i<MAX_INTER_BITMAP; i++){
		memset(data,0,gHoloSize);
		calImage(data, i*per_delt);
		int ret = write(fd,data,size);
		total += ret;
	}

	LOGE("save file %s(%d)",MATRIX_PATH,total);

	close(fd);
	
}



int readFile(uint8_t *dataList[]){
	int fd;
	int size = gHoloSize;
	char file[256];
	fd = open(MATRIX_PATH,O_RDONLY);
	float angle = ANGLE;
	if(fd < 0){
		LOGE("open1 %s err %d",MATRIX_PATH,errno);

		int fad = open("/sdcard/estartestA",O_RDONLY);
		if(fad < 0){
			LOGE("no angle profile!!1");
		}else{
			char abuf[5];
			memset(abuf,0,5);
			read(fad,abuf,4);
			int s = atoi(abuf);
			angle = (double)s/100.0;
			LOGFI("ANGLE:%f",angle);
		
		}
		angle  = getAngle(&gWidth);
		return getBitmapListMultiThread(dataList,angle);	
	}


	int ret = read(fd,dataList[0],gHoloSize*MAX_INTER_BITMAP);
	LOGFI("read matrix %d = gSize(%d) * %d",ret,gHoloSize,MAX_INTER_BITMAP);
	close(fd);
	if(ret != gHoloSize*MAX_INTER_BITMAP){
		LOGFI("read matrix error,ret = %d,errno:%d",ret,errno);
		return -1;
	}


	char *str = (char*)malloc(1024);
	int i, j;
	uint8_t *tmpbuffer;
	for(i=0; i<MAX_INTER_BITMAP; i++){
		tmpbuffer = dataList[i];
		memset(str,0,1024);
		char buf[4] = {'\0'};
		for(j=20000;j<20008;j++){
			sprintf(buf,"%x ",tmpbuffer[j]);
			strcat(str,buf);
		}
		LOGFI("buffer[%d]:%s",i,str);
	}
	free(str);
	return  0;
}	


int  getBitmapList(uint8_t *dataList[]){
	LOGXWL();
	int ret = readFile(dataList);
	if(ret == 0){
		return   0;
	}
	
	return -1;
}


void interleaveDeinit(uint8_t *dataList[]){
	LOGXWL();
	int i;
	for(i=0;i<MAX_INTER_BITMAP;i++){
		if( dataList[i] != NULL ) {
			dataList[i] = NULL;
		}
	}
	LOGXWL();
}


static void *ThreadWrapper(void *struData) {

   INTERDATA  *data = (INTERDATA*)struData;
   uint8_t **dataList = data->dataList;
   LOGFI("%d --> %d",data->start,data->end);
   
   int i;
   for (i=data->start;i<=data->end;i++){
   	LOGFI("%p",dataList[i]);
	calImage(dataList[i], i*per_delt);
   }
}



int  getBitmapListMultiThread(uint8_t *dataList[],float angle){
	long t1 = getTimeStamp();
	LOGFI("angle :%f,%f,datalist %p  %p",angle,gWidth,dataList,dataList[0]);
	//save angle , may used by  saving profile.
	gAngle = angle;
	initGlobalARG(angle);

	int perThread = MAX_INTER_BITMAP/NUMTHREAD;

	int i;

	INTERDATA  *data[NUMTHREAD];
	
	for(i=0;i< NUMTHREAD;i++){

		data[i] = (INTERDATA*)malloc(sizeof(INTERDATA));
		data[i]->start = i*perThread;
		data[i]->end = (i+1)*perThread - 1;
		data[i]->dataList = dataList;
		
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&gThread[i], &attr, ThreadWrapper, data[i]);
		pthread_attr_destroy(&attr);

	}

	for(i=0;i< NUMTHREAD;i++){
		pthread_join(gThread[i],NULL);
	//	free(data[i]);
	}
	
	int fd;
	int ret = system("mkdir  /sdcard/Android");   //make sure  /sdcard/Android is exists;

	fd = open(MATRIX_PATH,O_RDWR | O_CREAT,0777);
	if(fd < 0){
		LOGE("open %s err %d",MATRIX_PATH,errno);
		return errno;
	}

	ret = write(fd,dataList[0],gHoloSize*MAX_INTER_BITMAP);
	close(fd);	

	LOGE("save file %s(%d)",MATRIX_PATH,ret);

	LOGFI("took:%ld",getTimeStamp()-t1);

	return 0;
}


