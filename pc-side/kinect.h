#ifndef __KINECT_H__
#define __KINECT_H__

#ifdef _WIN32

#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include <AR/gsub.h>
//#include <AR/video.h>
#include "knVideo.h"
#include <AR/param.h>
#include <AR/ar.h>
#include <iostream>
#include "State.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define MAX_POLYGON_NUM 500

using namespace std;
using namespace cv;

class Kinect
{
public:
  Kinect();
  ~Kinect(){
    delete frontView;
    delete realWorldView;
    delete realToProjectX;
    delete realToProjectY;
    delete obstacleX;
    delete obstacleY;
    cleanup();
  };

  static Kinect* instance();

  void init();
  
  void cleanup(void);
  void keyEvent( unsigned char key, int x, int y);
  //void mainLoop(void);
  //void draw( void );
  void grabImg();
  void setTarget(Node *);
  float collisionDetect();
  bool targetDetect();
  bool targetDistance(float* x, float* y, float* z);
  void drawTarget();

    ARUint8         *dataPtr;
    unsigned short *depthPtr;
    //unsigned char  *depthPtrFloat;
    unsigned char *frontView;
    float *realWorldView;
    int *realToProjectX;
    int *realToProjectY;
    //cv::Mat *depthImg;

    int obstaclePolyNum;
    float *obstacleX;
    float *obstacleY;

    ARMarkerInfo    *marker_info;
    ARMarkerInfo    *detectMarker;
    int             marker_num;

    int             xsize, ysize;
    int             thresh;
    int             count;
    char			*vconf;
    char           *cparam_name;
    ARParam         cparam;

    int             patt_id;

};

#endif