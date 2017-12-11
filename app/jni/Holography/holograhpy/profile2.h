#ifndef  __PROFILE__H
#define __PROFILE__H

typedef struct  HolographyProfileHeader{
	uint32_t header;     	//header  must be 0x4f4c4f48
	double angle;      	//Fit  angle
	uint32_t baseY;      	// base Y
	uint32_t baseView;   	// base View
	uint32_t num;        	//number of Calibration data
	float *data;
}HolographyProfile;

void saveJZData(char *data);

void saveProfileFileN(double an, int16_t basey, int16_t baseview, float *data, int num);

int readProfileFileInternal(char *path,HolographyProfile *profile);

int readProfileFile(HolographyProfile *profile);

float getAngle();

//#define LOGFI(x, ...)   printf("%s(%d), "x"\n",__FUNCTION__,__LINE__, ##__VA_ARGS__);
//#define LOGXWL(x, ...)   printf("%s(%d), "x"\n",__FUNCTION__,__LINE__, ##__VA_ARGS__);

#endif
