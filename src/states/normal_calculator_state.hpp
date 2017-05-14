#ifndef __PROCALC_STATES_NORMAL_CALCULATOR_STATE_HPP__
#define __PROCALC_STATES_NORMAL_CALCULATOR_STATE_HPP__


#include "app_state.hpp"


struct NormalCalculatorState : public AppState {
  virtual void setup(int rootState) override {
    openCount = 10 - rootState;
  }

  int openCount = 10;
};


#endif
