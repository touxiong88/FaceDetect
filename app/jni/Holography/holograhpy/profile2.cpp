
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "profile.h"
#include "fflog.h"
#include "config.h"



// double gAngle = ANGLE;


void saveProfileFile(double angle, uint32_t baseY, uint32_t baseView, float *data, int num){
	LOGFI("Save Profile ***************************************");
	LOGFI("angle:			%f",angle);
	LOGFI("baseY:			%u",baseY);
	LOGFI("baseView:			%u",baseView);
	LOGFI("number of Calibration:		%d",num);

	system("mkdir /sdcard/Android");

	LOGFI("number of Calibration:		%d",num);

	char *path[3] ={(char*)PROFILE,(char*)PROFILE2,(char*)PROFILE3};

	int i;
	int fd;
	for (i = 0;  i < 3;i++){
		 fd = open(path[i],O_RDWR | O_CREAT | O_TRUNC, 0666);
		if(fd > 0){
			LOGFI("using:%s",path[i]);
			break;
		}else{
			LOGFI("can't open %s,err:%d",path[i],errno);
		}
	}


	if(fd < 0){
		return;
	}


	int header = 0x4f4c4f48;
	int datasize = num*sizeof(float);
	int ret = write(fd,&header,sizeof(uint32_t));
	ret += write(fd,&angle,sizeof(double));
	ret += write(fd,&baseY,sizeof(uint32_t));
	ret += write(fd,&baseView,sizeof(uint32_t));
	ret += write(fd,&num,sizeof(uint32_t));
	ret += write(fd,data,datasize);

	int le = 192 - ret;

	float tmp[192] = { 0 };
	ret += write(fd,tmp,le);

	LOGFI("written  %d, %d of them filled by '0'.",ret,le);
	
	close(fd);

	LOGFI("Save Profile ***************************************");


}

int readProfileFileInternal(char *path,HolographyProfile *profile){
	
	if(profile == NULL){
		LOGFI("profile is NULL,malloc space before  read profile");
		return -1;
	}

	int fd = open(path,O_RDONLY);
	if(fd < 0){
		LOGFI("opened error");
		return -1;
	}

	int ret = read(fd,&profile->header,sizeof(uint32_t));

	if(profile->header != 0x4f4c4f48){
		LOGFI("It is not an Holography  Profile!");
		LOGFI("%x",profile->header);
		close(fd);
		return -2;
	}


        int vv;

	ret += read(fd,&profile->angle,sizeof(double));
	ret += read(fd,&vv,sizeof(uint8_t));
	ret += read(fd,&profile->baseY,sizeof(uint32_t));
	ret += read(fd,&profile->baseView,sizeof(uint32_t));
	ret += read(fd,&profile->num,sizeof(uint32_t));

	int size = profile->num*sizeof(float);
	profile->data = (float*)malloc(size);
	

	int ret2 = read(fd,profile->data,size);
	ret += ret2;

	LOGFI("data size:		%d ",ret2);
	LOGFI("gblhpfh read:	%d\n",ret);

	close(fd);
	profile->angle = 17.78f;
	LOGFI("angle:			%f",profile->angle);
	LOGFI("baseY:			%u",profile->baseY);
	LOGFI("baseView:			%u",profile->baseView);
	LOGFI("number of Calibration:		%d",profile->num);


	return 0;
}

int  readProfileFile(HolographyProfile *profile){
	
	LOGXWL();

	int ret  = readProfileFileInternal(  (char*) PROFILE,profile);

	if(ret  == 0){
		LOGFI("use data");
		return  0;
	}

	ret = readProfileFileInternal( (char*)PROFILE2,profile);

	if(ret  == 0){
		LOGFI("use sdcard");
		return  0;
	}


	ret = readProfileFileInternal( (char*)PROFILE3,profile);

	if(ret  == 0){
		LOGFI("use android");
		return  0;
	}
	
	return -1;
}


float getAngle(){
	LOGFI("gAngle:%f",gAngle);
	double angle = ANGLE;
	HolographyProfile *profile = (HolographyProfile*)malloc(sizeof(HolographyProfile));
	int ret =  readProfileFile(profile);
	if(ret == 0){
		LOGFI("angle:%f",angle);
		angle = profile->angle;
	}
	char path[256] = "/sdcard/angle";
	int fd = open(path,O_RDONLY);
	if(fd < 0){
		LOGFI("find angle %s \n",path);
		return angle;
	}
	char buf[6] ;
	memset(buf,0,6);
	read(fd,buf,5);
	int tmp = atoi(buf);

	if(tmp <  1700 || tmp > 1900){
		LOGFI("angle:%f",angle);
		angle = (double)tmp/100.0;
	}
	
	close(fd);
	return angle;
}