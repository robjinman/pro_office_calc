#ifndef __PROCALC_MAIN_STATE_HPP__
#define __PROCALC_MAIN_STATE_HPP__


#include "utils.hpp"
#include "exception.hpp"
#include "app_state.hpp"
#include "state_ids.hpp"
#include "states/normal_calculator_state.hpp"
#include "states/danger_infinity_state.hpp"
#include "states/shuffled_keys_state.hpp"


#define REPLACE_WITH(UNIQUE_PTR, TYPE) \
  if (dynamic_cast<TYPE*>(UNIQUE_PTR.get()) == nullptr) { \
    UNIQUE_PTR.reset(new TYPE); \
  }

#define UNKNOWN_STATE_EXCEPTION(name, stateId) \
  EXCEPTION("Cannot initialise state/view " << name << " with state id " << state);

struct MainState : public AppState {
  virtual void setup(int stateId) override {
    rootState = stateId;

    if (ltelte<int>(ST_NORMAL_CALCULATOR_0, rootState, ST_NORMAL_CALCULATOR_10)) {
      REPLACE_WITH(m_substate, NormalCalculatorState);
    }
    else {
      switch (rootState) {
        case ST_DANGER_INFINITY:
          REPLACE_WITH(m_substate, DangerInfinityState);
          break;
        case ST_SHUFFLED_KEYS:
          REPLACE_WITH(m_substate, ShuffledKeysState);
          break;
        default:
          UNKNOWN_STATE_EXCEPTION("MainState", rootState);
          break;
      }
    }

    substate->setup(rootState);
  }

  int rootState;
  pAppState_t substate;
};


#endif
