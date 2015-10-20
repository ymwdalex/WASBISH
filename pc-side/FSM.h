/*
 *! Base class for states:
 *
 *  Based on the State pattern from "Design Patterns"
 */

#ifndef __FSM_H__
#define __FSM_H__

#include <iostream>

#include <assert.h>
#include "House.h"
#include "State.h"


class State;
class FSM;

//==========================================================================

class FSM{
  public:
    FSM(const std::string& inputFilename);
    virtual ~FSM();


    //! Get the NAS state
    State* getState() {
      assert(currState_ != NULL && "currState_ shall never be NULL");
      return currState_;
    };

    //! Get the NAS state
    const State* getState() const {
      assert(currState_ != NULL && "currState_ shall never be NULL");
      return currState_;
    };


    House* house;

    int readYAML(const std::string &inputFileName);

    //void collisionDetect();

    // true: target in the view;  false: out of the view

    void setFinishFlag(bool f);
    bool finish();

  //protected:


  //private:

    // Do not support copy c'tor
    FSM(const FSM&);

    // Do not support assignment operator
    FSM operator = (const FSM&);

    friend class State;
    friend class stateOutofSysc;
    friend class stateDetect;
    friend class stateCapture;
    friend class stateArrive;

    friend void switchState(FSM* const, State* const);
    friend void switchState(FSM* const, State* const,
        void (State::*fp)(FSM*));


    //! Change the NAS state
    void changeCurrState(State* state);

    //! Change the NAS state
    void changeCurrState(State* state, void (State::*fp)(FSM*));

    //! The current NAS state
    State* currState_;
    Node* currNode;
    bool finishFlag;
};

#endif