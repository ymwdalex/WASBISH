#include "monitor.h"
#include <iostream>
#include "stdio.h"
#include "kinect.h"
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

using namespace std; 

Kinect* kinect;
static unsigned char localBuffer[640 * 480 * 4]; 
static float obstacleX[MAX_POLYGON_NUM];
static float obstacleY[MAX_POLYGON_NUM];
int obstaclePolygonNum = 0;

void drawMarkerObstacle(void){
  if (kinect->detectMarker != NULL) {
    // blue Marker
    glColor3f(0.0f,0.0f,1.0f);
    ARMarkerInfo *marker = kinect->detectMarker;
    double x, y;
    glBegin(GL_POLYGON);
    for (int i=0; i<4; i++){
      x = marker->vertex[i][0];
      y = kinect->ysize - marker->vertex[i][1];
      glVertex2f(x, y);
    }
    glEnd();
  }

  int calibrateX = 0;
  int calibrateY = 0;
  if (obstaclePolygonNum > 0){
    // red obstacle
    glColor3f(1.0f,0.0f,0.0f);
    double x, y;
    glBegin(GL_POLYGON);
    for (int i=0; i<obstaclePolygonNum; i++){
      x = obstacleX[i] - calibrateX;
      y = kinect->ysize - obstacleY[i] - calibrateY;
      //y = obstacleY[i] - calibrateY;
      glVertex2f(x, y);
    }
    glEnd();
  }

}

static void mainLoop(void){
    argDrawMode2D();

    extern HANDLE mutexImgData;
    WaitForSingleObject(mutexImgData,INFINITE);  // no time-out interval
    memcpy(localBuffer, kinect->dataPtr, sizeof(localBuffer));
    ReleaseMutex(mutexImgData);

    extern HANDLE mutexObstacle;
    WaitForSingleObject(mutexObstacle,INFINITE); 
    obstaclePolygonNum = kinect->obstaclePolyNum;
    memcpy(obstacleX, kinect->obstacleX, sizeof(float)*obstaclePolygonNum);
    memcpy(obstacleY, kinect->obstacleY, sizeof(float)*obstaclePolygonNum);
    ReleaseMutex(mutexObstacle);

    argDispImage(localBuffer, 0,0 );

    // draw marker on the scene & polygon
    drawMarkerObstacle();

    argSwapBuffers();
}

DWORD WINAPI monitor( LPVOID lpParam ) 
{
  // get the handle of kinect
  kinect = Kinect::instance();

  /* open the graphics window */
  argInit( &(kinect->cparam), 1.0, 0, 0, 0, 0 );

  argMainLoop( NULL, NULL, mainLoop );

  return 0;
}
