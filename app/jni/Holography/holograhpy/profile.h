#ifndef  __PROFILE__H
#define __PROFILE__H

typedef struct  HolographyProfileHeader{
	uint32_t header;     	//header  must be 0x4f4c4f48
	double angle;      	//Fit  angle
	uint32_t bias;  
	double gwidth; 
	uint32_t cwidth;
	uint32_t cheight;
	uint32_t baseY;      	// base Y
	uint32_t baseView;   	// base View
	uint32_t num;        	//number of Calibration data
	float *data;
}HolographyProfile;

void saveProfileFile(double angle, double gwidth, int cwidth, int cheight, uint32_t baseY, uint32_t baseView, float *data, int num);

void saveProfileFile(double an, int16_t basey,int16_t baseview, float *data,int num);

void saveProfileFileT2(double angle, int baseY, int baseView, float *data, int num);

int readProfileFileInternal(char *path,HolographyProfile *profile);
int  readProfileFile(HolographyProfile *profile);
float getAngle(double *gwidth);


extern double gAngle;


#endif
