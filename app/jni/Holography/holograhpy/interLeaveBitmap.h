#ifndef __INTERLEAVEBITMAP__
#define __INTERLEAVEBITMAP__

void saveBitmap(double angle);
int   getBitmapList(uint8_t *data[]);
void interleaveDeinit(uint8_t *dataList[]);
int  getBitmapListWithAngle(uint8_t *dataList[],float angle);
int  getBitmapListTest(uint8_t *dataList[],float angle);
int  getBitmapListMultiThread(uint8_t *dataList[],float angle);


void initGlobalARG(double angle);
void calImage(uint8_t *data, double delt_s);



#endif
