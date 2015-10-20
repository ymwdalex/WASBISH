#include "socket.h"
#include "kinect.h"
#include "House.h"
#include <cv.h>
#include <highgui.h>
#include <vector>

using namespace cv;

//double          patt_width     = 80.0;
//double          patt_center[2] = {0.0, 0.0};
//double          patt_trans[3][4];

#define FRONTVIEW_XSIZE 800
#define FRONTVIEW_YSIZE 800
#define OBSTACLE_AREA 20


Kinect::Kinect():
  thresh(100),count(0),
  // camera configuration
  vconf("Data\\WDM_camera_flipV.xml"),
  cparam_name("Data/camera_para.dat"),
  obstaclePolyNum(0)
{
  frontView = new unsigned char[FRONTVIEW_XSIZE * FRONTVIEW_YSIZE];
  realWorldView = new float[FRONTVIEW_XSIZE * FRONTVIEW_YSIZE];
  realToProjectX = new int[FRONTVIEW_XSIZE * FRONTVIEW_YSIZE];
  realToProjectY = new int[FRONTVIEW_XSIZE * FRONTVIEW_YSIZE];
  obstacleX = new float[MAX_POLYGON_NUM];
  obstacleY = new float[MAX_POLYGON_NUM];
  init();
}

Kinect* Kinect::instance() {
  static Kinect kinect;
  return &kinect;
}

void Kinect::init()
{
    ARParam  wparam;
	
    /* open the video path */
    if( knVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( knVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

    //
    knVideoCapStart();

    // grab the depth image
    dataPtr = (ARUint8 *)knVideoGetImage();
    depthPtr = knVideoGetDepthRaw();
    //knVideoGetDepthRaw(&depthPtr, &depthPtrFloat);
}

void Kinect::grabImg()
{
    extern HANDLE mutexImgData;
    WaitForSingleObject(mutexImgData,INFINITE);  // no time-out interval
    /* grab a vide frame */
    dataPtr = (ARUint8 *)knVideoGetImage();
    ReleaseMutex(mutexImgData);

    // grab the depth image
    depthPtr = knVideoGetDepthRaw();
    //knVideoGetDepthRaw(&depthPtr, &depthPtrFloat);
}

void Kinect::setTarget(Node *node){

  if( (patt_id=arLoadPatt(node->markFileName.c_str())) < 0 ) {
        cout << "pattern load error !!" << endl;
        exit(0);
    }
}

//extern vector<vector <Point>> contours;
//vector <Point> obstaclePoints;
vector<vector <Point>> contours;
vector <Point> obstaclePoints;

float Kinect::collisionDetect(){
  int x, y;
  float xf, yf, zf;
  int viewX, viewY, viewZ;
  int midX = FRONTVIEW_XSIZE/2;
  int midY = FRONTVIEW_YSIZE/2;

  memset(frontView, 0, FRONTVIEW_XSIZE * FRONTVIEW_YSIZE * sizeof(unsigned char));
  memset(realWorldView, 0, FRONTVIEW_XSIZE * FRONTVIEW_YSIZE * sizeof(float));
  memset(realToProjectX, 0, FRONTVIEW_XSIZE * FRONTVIEW_YSIZE * sizeof(int));
  memset(realToProjectY, 0, FRONTVIEW_XSIZE * FRONTVIEW_YSIZE * sizeof(int));

  // ysize is row number, x size is column number
  for (x=0; x<ysize; x++){
    for (y=0; y<xsize; y++) {

      xf = (float)x; yf = (float)y;
      zf = (float)depthPtr[y*xsize + x];

      if (zf > 850)
        continue;

      KinectDepthToWorld(&xf, &yf, &zf);

      viewX = (int)xf; viewY = (int)yf; viewZ = (int)zf;
      
      //because of the field of the view, only detect x[-499..500] y[-499..500] z[350..1200]
      if (viewX >= -midX && viewX < midX)
        if (viewY >= -midY && viewY < midY)
          if ( viewZ <= 850){
            frontView[(viewX+midX) + (viewY+midY)*FRONTVIEW_XSIZE] = 0xff;
            realWorldView[(viewX+midX) + (viewY+midY)*FRONTVIEW_XSIZE] = zf;
            realToProjectX[(viewX+midX) + (viewY+midY)*FRONTVIEW_XSIZE] = viewX + midX;
            realToProjectY[(viewX+midX) + (viewY+midY)*FRONTVIEW_XSIZE] = viewY + midY;
          }
    }
  }

  Mat depthImg(FRONTVIEW_YSIZE, FRONTVIEW_XSIZE, CV_8UC1, frontView);
  //imwrite("last.png", depthImg);
  Mat depthDilate(FRONTVIEW_YSIZE, FRONTVIEW_XSIZE, CV_8UC1);
  Mat depthErode(FRONTVIEW_YSIZE, FRONTVIEW_XSIZE, CV_8UC1);
  float minD = 9999.1, d;


  // remove the "texture"
  dilate(depthImg, depthDilate, Mat());
  erode(depthDilate, depthErode, Mat());
  //imwrite("last_d.png", depthDilate);

  // get connected components
  findContours(depthErode, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  // remove noise & get minimal distance
  
  int minX = 0, minY = 0;
  int minIndex, i=0;
  vector<vector<Point>>::iterator c;
  for (c=contours.begin(); c!=contours.end(); c++, i++){
    //Mat tmp((const vector<Point>&) (*c));
    //if (contourArea(tmp) <= OBSTACLE_AREA){
    if ((*c).size() <= OBSTACLE_AREA){
      continue;
    } else {
      Point p = *((*c).begin());
      d = realWorldView[p.x + p.y*FRONTVIEW_XSIZE] / 1000.0;
      if (minD > d){
        minD = d; minX = p.x;  minY = p.y;
        minIndex = i;
      }     
    }
  }


  extern HANDLE mutexObstacle;
  if (minD < 9999.0) {
#if 0
    // convert to the project view
    vector<Point> obstacle;
    int x,y,xP,yP;
    WaitForSingleObject(mutexObstacle,INFINITE); 
    obstaclePolyNum = contours[minIndex].size() > MAX_POLYGON_NUM ? MAX_POLYGON_NUM : contours[minIndex].size();
    for (int i=0; i<contours[minIndex].size(); i++){
      x = (contours[minIndex])[i].x;
      y = (contours[minIndex])[i].y;
      xP = realToProjectX[x+y*FRONTVIEW_XSIZE];
      yP = realToProjectY[x+y*FRONTVIEW_XSIZE];
      //Point p(xP, yP);
      //obstacle.push_back(p);
      obstacleX[i] = xP;
      obstacleY[i] = yP;
    }
    ReleaseMutex(mutexObstacle);
    /*
    // approximate polygon
    Mat tmp(obstacle);
    approxPolyDP(tmp, obstaclePoints, 1,true);

    WaitForSingleObject(mutexObstacle,INFINITE); 
    obstaclePolyNum = obstaclePoints.size() > MAX_POLYGON_NUM ? MAX_POLYGON_NUM : obstaclePoints.size();
    for (int i=0; i<obstaclePolyNum; i++){
      obstacleX[i] = obstaclePoints[i].x;
      obstacleY[i] = obstaclePoints[i].y;
    }
    ReleaseMutex(mutexObstacle);
*/
#else 
    
    // approximate polygon
    Mat tmp((const vector<Point>&) (contours[minIndex]));
    approxPolyDP(tmp, obstaclePoints, 2,true);

    WaitForSingleObject(mutexObstacle,INFINITE); 
    obstaclePolyNum = obstaclePoints.size() > MAX_POLYGON_NUM ? MAX_POLYGON_NUM : obstaclePoints.size();
    for (int i=0; i<obstaclePolyNum; i++){
      obstacleX[i] = obstaclePoints[i].x;
      obstacleY[i] = obstaclePoints[i].y;
    }
    ReleaseMutex(mutexObstacle);
#endif
    
    int x = realToProjectX[minX+minY*FRONTVIEW_XSIZE];
    int y = realToProjectY[minX+minY*FRONTVIEW_XSIZE];
    int z = depthPtr[y*xsize + x];
    cout << "Distance:\t{" << minD << "}\t(" << z << ")\tX(" << minX << ")\tY(" << minY << ")\t" << endl;
  } else {
    WaitForSingleObject(mutexObstacle,INFINITE);
    obstaclePolyNum = 0;
    ReleaseMutex(mutexObstacle);
  }


  return minD;  
}

bool Kinect::targetDistance(float *x, float *y, float *z){
  if (detectMarker == NULL || depthPtr == NULL)
    return false;

  if (x==NULL || y==NULL || z==NULL)
    return false;

  *x = detectMarker->pos[0];
  *y = detectMarker->pos[1];
  int xAxis = (int)(*x);
  int yAxis = (int)(*y);
  *z = (float)depthPtr[yAxis*xsize + xAxis];

  printf("%.1f\t%.1f\t%.1f\n", *x, *y, *z);
  
  KinectDepthToWorld(x, y, z);

  return true;
}

bool Kinect::targetDetect(){
    int             j, k;

    /* detect the markers in the video frame */
    if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        cleanup();
        exit(0);
    }

    knVideoCapNext();

    /* check for object visibility */
    k = -1;
    for( j = 0; j < marker_num; j++ ) {
        if( patt_id == marker_info[j].id ) {
            if( k == -1 ) 
              k = j;
            else if( marker_info[k].cf < marker_info[j].cf )
              k = j;
        }
    }
    if( k == -1 ) {
        detectMarker = NULL;
        return false;
    }

    //cout << "Detect target" << k << endl;

    detectMarker = &marker_info[k];

    /* get the transformation between the marker and the real camera */
    //arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);

    //draw();

    //argSwapBuffers();


    return true;
}

/* cleanup function called when program exits */
void Kinect::cleanup(void)
{
    knVideoCapStop();
    knVideoClose();
    argCleanup();
}
