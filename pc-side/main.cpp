#ifdef _WIN32
// must before windows.h

#include "socket.h"
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

#include "House.h"
#include <iostream>
#include "mobileProtocol.pb.h"
#include "speech.h"
#include "FSM.h"
#include "monitor.h"
#include "collision.h"
#include <cv.h>
#include <highgui.h>
#include <ml.h>
#include <cxcore.h>
#include <cxtypes.h>
#include <time.h>

int exitCode;
HANDLE ghSemaphore;
HANDLE mutexImgData;
HANDLE mutexObstacle;
bool threadEndFlag = false;

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  FSM* fsm;

  ghSemaphore = CreateSemaphore( 
        NULL,           // default security attributes
        0,  // initial count
        1,  // maximum count
        NULL);          // unnamed semaphore

 mutexImgData = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

  mutexObstacle = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL); 

  if (argc == 1 ){
    fsm = new FSM("house.yaml");
  } else if (argc == 2) {
    fsm = new FSM(argv[1]);
  } else {
    cerr << "Usage: " << argv[0] << "[*.yaml]" << endl;
    cerr << "If don't input YAML file name, \"house.yaml\" is the default file" << endl;
    exit(1);
  }

  // create speech thread
  HANDLE handleSpeechThread = 0;
  handleSpeechThread = CreateThread(NULL,NULL,ttsThread,NULL,NULL,NULL);

  // create monitor thread
  HANDLE handleMonitorThread = 0;
  handleMonitorThread = CreateThread(NULL,NULL,monitor,NULL,NULL,NULL);

  // create obstacle detection thread
  HANDLE handleCollisionThread = 0;
  handleCollisionThread = CreateThread(NULL,NULL,obstacleDetect,NULL,NULL,NULL);

  // wait the thread ready
  WaitForSingleObject(ghSemaphore,INFINITE);

  State* state;
  unsigned int i = 0;
  while (!fsm->finish()) {
    state = fsm->getState();
    state->kinect->grabImg();
    state->stateAction(fsm);
    state->updateState(fsm);
    Sleep(40);
  }

  threadEndFlag = true;
  
  TerminateThread(handleMonitorThread, 0);
  TerminateThread(handleSpeechThread, 0);
  //TerminateThread(handleCollisionThread, 0);

  switch(WaitForSingleObject(handleCollisionThread, INFINITE)) {
	  case WAIT_OBJECT_0 :
		  // Thread exited, react somehow:
		  break;
	  default :
      cout << "WaitForSingleObject error!" << endl;
  };

  CloseHandle(handleMonitorThread);
  CloseHandle(handleSpeechThread);
  CloseHandle(handleCollisionThread);

  delete fsm;
}
