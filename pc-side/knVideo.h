//---"knVideo.h"---ここから-------------------------------------------------------

//2010.12.15 Ver.1.00 for OpenNI
extern int knVideoOpen(char *config);
extern int knVideoClose(void);
extern int knVideoCapStart(void);
extern int knVideoCapStop(void);
extern int knVideoCapNext(void);
extern int knVideoInqSize(int *x, int *y);
extern int knVideoDispOption(void);
extern unsigned char * knVideoGetImage(void);
//extern void knVideoGetDepthRaw(unsigned short ** , unsigned char**);
extern unsigned short *knVideoGetDepthRaw(void);
extern unsigned char * knVideoGetDepthColor(void);
extern unsigned char * knVideoGetDepthImage(void);
extern int knVideoSetMotorPosition(float MotorPosition);
extern int knVideoSetLedMode(int LedMode);
extern void KinectDepthToWorld(float *x, float *y, float *z);
extern void KinectDepthToProject(float *x, float *y, float *z);

//---"knVideo.h"---
