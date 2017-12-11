#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "profile.h"
#include "fflog.h"
#include "config.h"

#define PROFILE "/sdcard/HolographyProfile.txt"
double gAngle = ANGLE;

void saveProfileFile(double angle,double gwidth,int cwidth,int cheight,uint32_t baseY,uint32_t baseView, float *data,int num){
	LOGFI("Save Profile ***************************************");
	LOGFI("angle:			%f",angle);
	LOGFI("gwidth:			%f",gwidth);
	LOGFI("baseY:			%u",baseY);
	LOGFI("baseView:			%u",baseView);
	LOGFI("number of Calibration:	%d",num);
	
	FILE *fp = fopen(PROFILE,"w+");

	int opened = 1;
	if(!fp ){
		LOGFI("open1 %s err %d\n",PROFILE,errno);
	}
    fprintf(fp,"HOLO\n");
    fprintf(fp,"angle %f\n",angle);
    fprintf(fp,"bias %d\n",1);
    fprintf(fp,"gwidth %f\n",gwidth);
    fprintf(fp,"cwidth %d\n",cwidth);
    fprintf(fp,"cheight %d\n",cheight);	
    fprintf(fp,"baseY %d\n",baseY);
    fprintf(fp,"baseView %d\n",baseView);
    fprintf(fp,"data %d\n",num);

    LOGFI("num = %d   data = %x", num, data);

	for(int i = 0; i < num; i++){
		fprintf(fp,"%f\n",data[i]);
	}
	fprintf(fp,"end\n");
	

	fclose(fp);

	LOGFI("Save Profile ***************************************");
}

void saveProfileFileT2(double angle, int baseY, int baseView, float *data, int num){

	LOGFI("Save Profile ***************************************");
	LOGFI("angle:			%f", angle);
	LOGFI("baseY:			%u", baseY);
	LOGFI("baseView:			%u", baseView);
	LOGFI("number of Calibration:		%d", num);

	char file[256] = "/sdcard/HolographyProfile";

	int fd = open(file, O_RDWR | O_CREAT | O_TRUNC, 0666);

	int opened = 1;
	if (fd < 0){
		LOGFI("open1 %s err %d\n", file, errno);
	}

	int le = 192;
	int header = 0x4f4c4f48;


	int ret = write(fd, &header, sizeof(uint32_t));
	LOGFI("written  %d, %s.", ret, strerror(errno));
	ret += write(fd, &angle, sizeof(double));
	LOGFI("written  %d, %s.", ret, strerror(errno));
	ret += write(fd, &baseY, sizeof(uint32_t));
	LOGFI("written  %d, %s.", ret, strerror(errno));
	ret += write(fd, &baseView, sizeof(uint32_t));
	LOGFI("written  %d, %s.", ret, strerror(errno));
	ret += write(fd, &num, sizeof(uint32_t));
	LOGFI("written  %d, %s.", ret, strerror(errno));
	for (int i = 0; i < num; i++){
		//	LOGFI("[%d]:%x",i,data[i]);
	}

	int datasize = num*sizeof(float);

	ret += write(fd, data, datasize);
	LOGFI("%p", data);
	LOGFI("written  %d, %s.", ret, strerror(errno));


	if (ret < 192){
		le = 192 - ret;
		float tmp[192] = { 0 };
		ret += write(fd, tmp, le);
		LOGFI("written  %d, %d of them filled by '0'.", ret, le);
	}
	close(fd);

	LOGFI("Save Profile ***************************************");
}


void printProfile(HolographyProfile *profile){

	LOGFI("angle:%f",profile->angle);
	LOGFI("bias:%d",profile->bias);
	LOGFI("baseY:%d",profile->baseY);
	LOGFI("baseView:%d",profile->baseView);
	LOGFI("num:%d",profile->num);


	for(int i = 0;  i < 5; i++){
		LOGFI("[%2d]:%f",i,profile->data[i]);	
	}
}

int readProfileFile(char *name , HolographyProfile *profile){
	if(profile == NULL){
		profile = new  HolographyProfile();
	}

	FILE *fp = fopen(name,"r");


	if(!fp ){
		LOGFI("open1 %s err %d\n",PROFILE,errno);
             return -1;
	}
	char head[64];
	double angle;
	int bias,baseY,baseView,num;

	char buf[256];

	//while(fgets() != NULL){

	//}

	fgets(buf,256,fp);
	sscanf(buf,"%s",head);

      LOGFI("%s",head);
    
	if(head[0] != 'H' || head[1] != 'O'|| head[2] != 'L' || head[3] != 'O'){

		LOGFI("not holo profile! %s, %s",head,name);
	}


	fgets(buf,256,fp);
	sscanf(buf,"%s %lf",head,&profile->angle);
	LOGFI("%s %f",head,profile->angle);

	fgets(buf,256,fp);
	sscanf(buf,"%s %d",head,&profile->bias);
	LOGFI("%s %d",head,profile->bias);
       profile->gwidth = gWidth;
    
// /*
	fgets(buf,256,fp);
	sscanf(buf,"%s %lf",head,&profile->gwidth);
	LOGFI("%s %lf",head,profile->gwidth);

    fgets(buf,256,fp);
	sscanf(buf,"%s %d",head,&profile->cwidth);
	LOGFI("%s %d",head,profile->cwidth);

    fgets(buf,256,fp);
	sscanf(buf,"%s %d",head,&profile->cheight);
	LOGFI("%s %d",head,profile->cheight);
// */

	fgets(buf,256,fp);
	sscanf(buf,"%s %d",head,&profile->baseY);
	LOGFI("%s %d",head,profile->baseY);

	fgets(buf,256,fp);
	sscanf(buf,"%s %d",head,&profile->baseView);
	LOGFI("%s %d",head,profile->baseView);

	fgets(buf,256,fp);
	sscanf(buf,"%s %d",head,&profile->num);
	LOGFI("%s %d",head,profile->num);


	int size = profile->num*sizeof(float);
	float *data = (float*)malloc(size);
	profile->data = data;


	for(int i = 0; i < profile->num; i++){
		fgets(buf,256,fp);
		sscanf(buf,"%f",data+i);	
//		LOGV("readProfileFile[%d] = %f", i, data[i]);
	}
	fclose(fp);

        return 0;
}





int  readProfileFile(HolographyProfile *profile){

    	int ret  = readProfileFile(  (char*) PROFILE,profile);
	if(ret  == 0){
		LOGFI("use data");
             printProfile(profile);
	}
      
       return ret;


    
}

int main(){

	HolographyProfile profile;
	readProfileFile((char*)PROFILE,&profile);
	printProfile(&profile);
	return 0;
	int i;

	double angle = 17.800;
	int baseY = 300;
	int baseView = 0;

	float  modata[] ={
    652,646,642,637,632,626,
    622,617,611,607,601,597,
    591,586,581,576,570,566,
    561,555,551,545,540,535,
    530,525,520,514,509,504,
    498,494,488,483,478,473,
    468,462,457,452,447,442,
    436,431,425,419,415,410,
    403,399,393,387,382,377,
    371,366,360,355,348,343,
    338,332,326,321,315,309,
    304,297,291,286,279,274,
    268,262,256,250,244,238,
    232,226,220,213,208,201,
    195,189};


	uint16_t  *ViewID;


	int size = sizeof(modata)/sizeof(modata[0]);
	float  *modata2 = (float*)malloc(size*sizeof(modata[0]));
	uint16_t  *ViewArr = (uint16_t*)malloc(size*sizeof(modata[0]));
	
	for(i = 0; i < size; i++){
	     modata2[i] = modata[size - 1 - i];
	     ViewArr[i] = baseView%32;
	     baseView++;

	}
	baseView = baseView%32;

	angle = 17.850;
	baseView = 4;
//move left then baseview++
	saveProfileFile(angle,gWidth,800,600,baseY,baseView,modata,size);

    	return 0;
}



float getAngle(double *gwidth){
	LOGFI("gAngle:%f",gAngle);
	double angle = ANGLE;
	HolographyProfile *profile = (HolographyProfile*)malloc(sizeof(HolographyProfile));
	int ret =  readProfileFile(profile);
	if(ret == 0){
		LOGFI("angle:%f",angle);
		angle = profile->angle;
		*gwidth = profile->gwidth;
              
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

	if(tmp >  1700 &&  tmp < 1800){
		LOGFI("angle:%f",angle);
		angle = (double)tmp/100.0;
	}
	
	close(fd);
	return angle;
}
















