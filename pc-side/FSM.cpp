/*!
  \file     State.cpp
  \brief    Base class for states
*/

/*---------------------------------------------------
 *   Include files
 * --------------------------------------------------*/
#include "FSM.h"
#include "yaml/yaml.h"
#include <ctype.h>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include "State.h"

using namespace std;


//=====================================================================

FSM::FSM(const std::string& inputFilename) : currState_(stateInit::instance()) {
  stateInit::instance();
  stateOutofSysc::instance();
  stateDetect::instance();
  stateCapture::instance();
  stateArrive::instance();
      
  house = new House();

  readYAML(inputFilename);
  house->search();
  //house->getShortestPath();
  //currNode = NULL;
  currNode = house->getStartPoint();
  finishFlag = false;

  //currTarget = house->ge

  // Make sure that all states are created

}

FSM::~FSM() {
  delete house;
}

bool FSM::finish() {
  return finishFlag;
}

void FSM::setFinishFlag(bool f){
  finishFlag = f;
}

void FSM::changeCurrState(State* state) {
  assert(currState_ != NULL);
  assert(state != NULL);
  
  cout << "Change state:" << currState_->getStateName() << "->" << state->getStateName() << endl;
  
  currState_ = state;
}

void FSM::changeCurrState(State* currState, void (State::*fp)(FSM*))
{
  changeCurrState(currState);
  (currState_->*fp)(this);
}

/*
void FSM::collisionDetect(){
  cout << "Detect collision" << endl;
}
*/

int FSM::readYAML(const std::string &inputFileName) {
    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFileName.c_str());
    if (!fin) {
        cerr << "Error: unable to open " << inputFileName << " for reading." << endl;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);

            if (const YAML::Node * pNode = doc.FindValue("Nodes")){
                for (YAML::Iterator it = pNode->begin(); it != pNode->end(); it++) {
                  int index;
                  std::string name, markFileName;
                  (*it)["index"] >> index;
                  (*it)["name"] >> name;
                  (*it)["markerFileName"] >> markFileName;
                  house->addNode(index, name, markFileName);
                }
            }

            if (const YAML::Node * pEdge = doc.FindValue("Paths")){
                for (YAML::Iterator it = pEdge->begin(); it != pEdge->end(); it++) {
                  int id1, id2;
                  int weight = 1;
                  (*it)["point1"] >> id1;
                  (*it)["point2"] >> id2;
                  if (pEdge->FindValue("weight")) {
                    (*it)["weight"] >> weight;
                  }
                  
                  house->addPath(id1, id2, weight);
                }
            }

            if (const YAML::Node *pId = doc.FindValue("StartPoint")){
                int point;
                *pId >> point;
                if (!house->setStartPoint(point)){
                  cout << "set Start point fail!" << endl;
                  return false;
                }
            } else {
              cout << "No startPoint in YAML file" << endl;
              return false;
            }

            if (const YAML::Node * pId = doc.FindValue("EndPoint")){
                int point;
                *pId >> point;
                if (!house->setEndPoint(point)){
                  cout << "set End point fail!" << endl;
                  return false;
                }
            } else {
              cout << "No EndPoint in YAML file" << endl;
              return false;
            }
        } else {
          return false;
        }
    } catch (YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }

    cout << "YAML parsing results: " << endl;

    return true;
}

//=========================================================================

