#include "socket.h"
#include "State.h"
#include <iostream>
#include "stdio.h"
#include <assert.h>
#include "math.h"
#include <windows.h>

SocketClient sockSpeech("127.0.0.1", 8765);

State::State(const char* const stateName):
  stateName_(stateName),
  stateCounter(0),targetInView(0),targetOutOfView(0),
  // every 25 clock circle, run action/update once, except stateCapture
  stateFrequency(25)
{
  assert(stateName != NULL);
  //State::speech = new Speech();
  //speech = Speech::instance();
  kinect = Kinect::instance();
}

State::State(const char* const stateName, unsigned int frequency):
  stateName_(stateName),
  stateCounter(0),targetInView(0),targetOutOfView(0),
  // every 25 clock circle, run action/update once, except stateCapture
  stateFrequency(frequency)
{
  assert(stateName != NULL);
  //State::speech = new Speech();
  //speech = Speech::instance();
  kinect = Kinect::instance();
}

State::~State() {
}

//=====================================================================

stateInit::stateInit():
	State("stateInit") {
}

stateInit::~stateInit() {
}

stateInit* stateInit::instance() {
  static stateInit state;
  return &state;
}

void stateInit::stateAction(FSM *fsm){
  
}

void stateInit::updateState(FSM* fsm){
  sockSpeech.SendBytes(new std::string("Begin working."));

  // set next target
  kinect->setTarget(fsm->currNode);

  // InitState ==> stateOutofSysc
  fsm->changeCurrState(stateOutofSysc::instance());

  // when enter in stateOutofSysc, speak "target is not in the view"
  Node* currNode = fsm->currNode;
  char sound[1024];
  int len = sprintf(sound, "Next is %s", currNode->name.c_str());
  sockSpeech.SendBytes(sound, len);
}

unsigned int stateInit::getDelayTime(){
  return delayTime;
}


//=====================================================================

stateOutofSysc::stateOutofSysc():
	State("stateOutofSysc") {
}

stateOutofSysc::~stateOutofSysc() {
}

stateOutofSysc* stateOutofSysc::instance() {
  static stateOutofSysc state;
  return &state;
}

void stateOutofSysc::stateAction(FSM *fsm){
  bool result = kinect->targetDetect();

  // every 1s
  if (notExecuteAction())
    return;

  // Detect target
  if (result){
    targetInView++;
  } else {
    targetOutOfView++;
  }

  return;
}

void stateOutofSysc::updateState(FSM *fsm){
  // switch to "unstable" state
  if (targetInView > 0){
    // clean counter
    cleanCounter();
    fsm->changeCurrState(stateDetect::instance());
  }
}

unsigned int stateOutofSysc::getDelayTime(){
  return delayTime;
}

//=====================================================================

stateDetect::stateDetect():
	State("stateDetect") {
}

stateDetect::~stateDetect() {
}

stateDetect* stateDetect::instance() {
  static stateDetect state;
  return &state;
}

void stateDetect::stateAction(FSM *fsm){
  bool result = kinect->targetDetect();

  // every 1s
  if (notExecuteAction())
    return;

  // Detect target
  if (result){
    targetInView++;
    // clear the counterpart counter
    targetOutOfView = 0;
  } else {
    targetOutOfView++;
    // clear the counterpart counter
    targetInView = 0;
 }

  return;
}

void stateDetect::updateState(FSM *fsm){
  // continue detect 3 times, switch to "capture" state
  if (targetInView >= 1){
    // clean counter
    cleanCounter();
    fsm->changeCurrState(stateCapture::instance());

    // speak the distance
    Node* currNode = fsm->currNode;
    char sound[1024];
    int len = sprintf(sound, "Capture %s", currNode->name.c_str());
    sockSpeech.SendBytes(sound, len);
    return;
  } 
  
  // continue failed to detect 3 times, switch to "stateOutofSysc" state
  if (targetOutOfView >= 2) {
    // clean counter
    cleanCounter();
    fsm->changeCurrState(stateOutofSysc::instance());  

    // when enter in stateOutofSysc, speak "target is not in the view"
    Node* currNode = fsm->currNode;
    char sound[1024];
    int len = sprintf(sound, "%s is not in the view", currNode->name.c_str());
    sockSpeech.SendBytes(sound, len);

    return;
  }
}

unsigned int stateDetect::getDelayTime(){
  return delayTime;
}

//=====================================================================

stateCapture::stateCapture(): 
	State("stateCapture", 60), distance(999.9){
}

stateCapture::~stateCapture() {
}

stateCapture* stateCapture::instance() {
  static stateCapture state;
  return &state;
}

void stateCapture::stateAction(FSM *fsm){
  bool result = kinect->targetDetect();
  char sound[1024];

  // every 2s
  if (notExecuteAction())
    return;

   // Detect target
  if (result){
    targetInView++;

    // Detect distance
    if (kinect->targetDistance(&x, &y, &z)){
      distance = sqrt(x*x+y*y+z*z)/1000.0;
      // when distance less than 0.5, it is 0.0
      if (distance > 0.4){
        // get angle
        char* lOrR;
        if (x > 0){
          lOrR = "right";
        } else {
          lOrR = "left";
        }

        float angle = atan(x/z) * 180 / 3.14159265;
        int angleInt = abs((int)angle);
        int len;
        if (angleInt >= 15 ){
          len = sprintf(sound, "%.1f meter, %d degree %s", distance, angleInt, lOrR);
        } else {
          len = sprintf(sound, "%.1f meter", distance, angleInt, lOrR);
        }
        sockSpeech.SendBytes(sound, len);
      }
    } else {
      sockSpeech.SendBytes(new std::string("distance detect failed."));
      distance = 999.9;
    }
  } else {
    targetOutOfView++;
  }
}

void stateCapture::updateState(FSM *fsm){
  // switch to "unstable" state
  if (targetOutOfView > 0){
    // clean distance
    distance = 999.9;

    // clean counter
    cleanCounter();
    fsm->changeCurrState(stateDetect::instance());
    return;
  }

  // reach the target
  if (distance < 1.05) {
    cout << "arrive current node, distance:\t" << distance << endl;
    if (fsm->currNode == fsm->house->destination){
      // arrive
      cleanCounter();
      fsm->changeCurrState(stateArrive::instance());
      //kinect->collisionDetect();
    } else {
      // next target
      cleanCounter();

      // clean distance
      distance = 999.9;

      // update next target;
      Node* currNode = fsm->currNode;
      Node* nextNode = fsm->house->getNextNode(currNode);
      fsm->currNode = nextNode;

      // speak
      char sound[1024];
      int len = sprintf(sound, "Arrive %s, next is %s", currNode->name.c_str(), nextNode->name.c_str());
      sockSpeech.SendBytes(sound, len);

      // set next target
      kinect->setTarget(fsm->currNode);

      // change state
      fsm->changeCurrState(stateOutofSysc::instance());
    }
  }

  return;
}

unsigned int stateCapture::getDelayTime(){
  return delayTime;
}

//=====================================================================

stateArrive::stateArrive():
	State("stateArrive") {
}

stateArrive::~stateArrive() {
}

stateArrive* stateArrive::instance() {
  static stateArrive state;
  return &state;
}

void stateArrive::stateAction(FSM *fsm){
  // speak
  char sound[1024];
  int len = sprintf(sound, "Destination %s arrive, mission complete!", fsm->currNode->name.c_str());
  sockSpeech.SendBytes(sound, len);

  // let speech thread speak out
  Sleep(8000);

  fsm->setFinishFlag(true);
}

void stateArrive::updateState(FSM *fsm){

}

unsigned int stateArrive::getDelayTime(){
  return delayTime;
}