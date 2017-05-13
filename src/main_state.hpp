#ifndef __PROCALC_MAIN_STATE_HPP__
#define __PROCALC_MAIN_STATE_HPP__


#include "app_state.hpp"
#include "state_ids.hpp"
#include "exception.hpp"
#include "utils.hpp"


struct MainSub0State : public AppState {
  virtual void initialise(int rootState) override {
    if (!ltelte<int>(ST_NORMAL_CALCULATOR_0, rootState, ST_NORMAL_CALCULATOR_10)) {
      EXCEPTION("MainSub0State cannot be initialised for rootState " << rootState);
    }

    openCount = 10 - rootState;
  }

  int openCount;
};

struct MainSub1State : public AppState {
  virtual void initialise(int rootState) override {
    if (rootState != ST_DANGER_INFINITY) {
      EXCEPTION("MainSub1State cannot be initialised for rootState " << rootState);
    }

    divByZero = false;
  }

  bool divByZero;
};

struct MainSub2State : public AppState {
  virtual void initialise(int rootState) override {
    if (rootState != ST_SHUFFLED_KEYS) {
      EXCEPTION("MainSub2State cannot be initialised for rootState " << rootState);
    }

    a = 5;
  }

  int a;
};

struct MainState : public AppState {
  virtual void initialise(int stateId) override {
    rootState = stateId;

    if (ltelte<int>(ST_NORMAL_CALCULATOR_0, rootState, ST_NORMAL_CALCULATOR_10)) {
      subState.reset(new MainSub0State);
    }
    else {
      switch (rootState) {
        case ST_DANGER_INFINITY:
          subState.reset(new MainSub1State);
          break;
        case ST_SHUFFLED_KEYS:
          subState.reset(new MainSub2State);
          break;
        default:
          EXCEPTION("MainState cannot be initialised for rootState " << rootState);
          break;
      }
    }

    subState->initialise(rootState);
  }

  int rootState;
  pAppState_t subState;
};


#endif
