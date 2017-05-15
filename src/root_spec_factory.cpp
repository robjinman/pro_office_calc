#include "root_spec_factory.hpp"
#include "exception.hpp"
#include "state_ids.hpp"
#include "states/normal_calculator_state.hpp"
#include "states/danger_infinity_state.hpp"
#include "states/shuffled_keys_state.hpp"


RootSpec* makeRootSpec(int stateId) {
  switch (stateId) {
    case ST_NORMAL_CALCULATOR_0:
    case ST_NORMAL_CALCULATOR_1:
    case ST_NORMAL_CALCULATOR_2:
    case ST_NORMAL_CALCULATOR_3:
    case ST_NORMAL_CALCULATOR_4:
    case ST_NORMAL_CALCULATOR_5:
    case ST_NORMAL_CALCULATOR_6:
    case ST_NORMAL_CALCULATOR_7:
    case ST_NORMAL_CALCULATOR_8:
    case ST_NORMAL_CALCULATOR_9:
      return normal_calculator::makeRootSpec(stateId);
    case ST_DANGER_INFINITY:
      return danger_infinity::makeRootSpec();
    case ST_SHUFFLED_KEYS:
      return shuffled_keys::makeRootSpec();
    default:
      EXCEPTION("Unrecognised state id");
  }
}
