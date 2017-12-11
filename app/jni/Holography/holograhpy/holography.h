#ifndef __HOLOGRAPHY__
#define  __HOLOGRAPHY__



int holographyInit(int width, int height);
void holographyDeinit();

void updateImage();
int setAngle(float angle);
void sendDelt(int n);
void sendDeltFromAPP(int n);


int initRaserData(int screenWidth, int screenHeight);
int calRaserWidth(float angle, float raserWidth);
void updateRaserTexture();


#endif