#include "f_main_spec_factory.hpp"
#include "exception.hpp"
#include "state_ids.hpp"
#include "states/st_normal_calc.hpp"
#include "states/st_danger_infinity.hpp"
#include "states/st_shuffled_keys.hpp"
#include "states/st_login_screen.hpp"
#include "fragments/f_main/f_main_spec.hpp"


FMainSpec* makeFMainSpec(int stateId) {
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
      return st_normal_calc::makeFMainSpec(stateId);
    case ST_DANGER_INFINITY:
      return st_danger_infinity::makeFMainSpec(stateId);
    case ST_SHUFFLED_KEYS:
      return st_shuffled_keys::makeFMainSpec(stateId);
    case ST_LOGIN_SCREEN:
      return st_login_screen::makeFMainSpec(stateId);
    default:
      EXCEPTION("Unrecognised state id");
  }
}
