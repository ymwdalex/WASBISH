#ifndef __STATE_H__
#define __STATE_H__

#include "House.h"
#include "Speech.h"
#include "kinect.h"
#include "FSM.h"

class State;
class FSM;
class Kinect;

class State {
  public:
    //Speech* speech;
    Kinect* kinect;

    // State actions

    // counters
    unsigned int stateCounter;
    unsigned int stateFrequency;
    unsigned int targetInView;
    unsigned int targetOutOfView;
    unsigned int delayTime;

    virtual void updateState(FSM *) = 0;
    virtual unsigned int getDelayTime() = 0;
    virtual void stateAction(FSM *) = 0;

    virtual void cleanCounter(){
      stateCounter = 0;
      targetInView = 0;
      targetOutOfView = 0;
    }
    
    virtual bool notExecuteAction(){
      stateCounter++;
      if (stateCounter > stateFrequency) {
        stateCounter = 0;
        return false;
      } else {
        return true;
      }
    }

    virtual void setPollPeriod(unsigned int period){
      stateFrequency = period;
    }

    //! Return state name
    virtual const char* const getStateName() const {
      return stateName_;
    }

protected:
    // Objects can only be constructed by sub classes.
    // Let the default c'tor be protected
    State(const char* const);
    State(const char* const, unsigned int);
    // D'tor
    virtual ~State();
  private:
    const char* const stateName_;
    // Non supported copy c'tor
    State(const State&);
    // Non supported assignment operator
    //State operator = (const State&);
};

//==============================================

class stateInit: public State {

  public:
    //! Get singleton instance
    static stateInit* instance();
    virtual void updateState(FSM *);
    virtual unsigned int getDelayTime();
    virtual void stateAction(FSM *);

private:
    // As the class is a singleton keep the c'tors private
    stateInit();

    // D'tor
    virtual ~stateInit();

    // Non supported copy c'tor
    stateInit(const stateInit&);

    // Non supported assignment operator
    stateInit operator = (const stateInit&);
};


//==============================================

class stateOutofSysc: public State {

  public:
    //! Get singleton instance
    static stateOutofSysc* instance();
    virtual void updateState(FSM *);
    virtual unsigned int getDelayTime();
    virtual void stateAction(FSM *);

private:
    // As the class is a singleton keep the c'tors private
    stateOutofSysc();

    // D'tor
    virtual ~stateOutofSysc();

    // Non supported copy c'tor
    stateOutofSysc(const stateOutofSysc&);

    // Non supported assignment operator
    stateOutofSysc operator = (const stateOutofSysc&);
};

class stateDetect: public State {

  public:
    //! Get singleton instance
    static stateDetect* instance();
    virtual void updateState(FSM *);
    virtual unsigned int getDelayTime();
    virtual void stateAction(FSM *);

private:
    // As the class is a singleton keep the c'tors private
    stateDetect();

    // D'tor
    virtual ~stateDetect();

    // Non supported copy c'tor
    stateDetect(const stateDetect&);

    // Non supported assignment operator
    stateDetect operator = (const stateDetect&);
};


class stateCapture: public State {

  public:
    //! Get singleton instance
    static stateCapture* instance();
    virtual void updateState(FSM *);
    virtual unsigned int getDelayTime();
    virtual void stateAction(FSM *);

private:
    // As the class is a singleton keep the c'tors private
    stateCapture();

    // D'tor
    virtual ~stateCapture();

    // Non supported copy c'tor
    stateCapture(const stateCapture&);

    // Non supported assignment operator
    stateCapture operator = (const stateCapture&);

    float distance;
    // coordinate
    float x, y, z;
};


class stateArrive: public State {

  public:
    //! Get singleton instance
    static stateArrive* instance();
    virtual void updateState(FSM *);
    virtual unsigned int getDelayTime();
    virtual void stateAction(FSM *);

private:
    // As the class is a singleton keep the c'tors private
    stateArrive();

    // D'tor
    virtual ~stateArrive();

    // Non supported copy c'tor
    stateArrive(const stateArrive&);

    // Non supported assignment operator
    stateArrive operator = (const stateArrive&);
};


#endif