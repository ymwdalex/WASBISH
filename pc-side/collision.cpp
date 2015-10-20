#include "socket.h"
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
#include "collision.h"
#include "mobileProtocol.pb.h"

using namespace std; 

SocketClient sockVibration("192.168.1.101", 9876);
SocketClient sockSpeech1("127.0.0.1", 8765);

long vNear[] = {0,200,200};
long vMid[] = {0,500,200};
long vFar[] = {0,600,200};

#define D_FAR 0.6
#define D_MID 0.6
#define D_NEAR 

DWORD WINAPI obstacleDetect( LPVOID lpParam ) 
{
  // get the handle of kinect
  Kinect* kinect = Kinect::instance();
  float d;
  mobileProtocol::ucMsg *msg = new mobileProtocol::ucMsg();

  while (!threadEndFlag){
    Sleep(2000);
    d = kinect->collisionDetect();
    if (d > 0.85)
      continue;

    char sound[1024];
    int len = sprintf(sound, "object at %.1f meter", d);
    sockSpeech1.SendBytes(sound, len);

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    msg->set_type(mobileProtocol::CmdType::VIBRATION);
    if (d > D_FAR){
      for (int i=0; i<sizeof(vFar)/sizeof(long); i++){
        msg->add_vpattern(vFar[i]);
      }
    //} else if ( d <= D_FAR && d > D_MID) {
    //  for (int i=0; i<sizeof(vMid)/sizeof(long); i++){
    //    msg->add_vpattern(vMid[i]);
    //  }
    } else {
      for (int i=0; i<sizeof(vNear)/sizeof(long); i++){
        msg->add_vpattern(vNear[i]);
      }
    }
    
    std::string serialMsg;
    msg->SerializeToString(&serialMsg);

    sockVibration.SendBytes(&serialMsg);
  }
  
  delete msg;

  return 0;
}
