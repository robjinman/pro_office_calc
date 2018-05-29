#include "f_main_spec_factory.hpp"
#include "exception.hpp"
#include "fragments/f_main/f_main_spec.hpp"
#include "state_ids.hpp"
#include "states/st_normal_calc.hpp"
#include "states/st_danger_infinity.hpp"
#include "states/st_shuffled_keys.hpp"
#include "states/st_are_you_sure.hpp"
#include "states/st_its_raining_tetrominos.hpp"
#include "states/st_making_progress.hpp"
#include "states/st_youve_got_mail.hpp"
#include "states/st_going_in_circles.hpp"
#include "states/st_millennium_bug.hpp"
#include "states/st_back_to_normal.hpp"
#include "states/st_test.hpp"


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
    case ST_ARE_YOU_SURE:
      return st_are_you_sure::makeFMainSpec(stateId);
    case ST_ITS_RAINING_TETROMINOS:
      return st_its_raining_tetrominos::makeFMainSpec(stateId);
    case ST_MAKING_PROGRESS:
      return st_making_progress::makeFMainSpec(stateId);
    case ST_YOUVE_GOT_MAIL:
      return st_youve_got_mail::makeFMainSpec(stateId);
    case ST_GOING_IN_CIRCLES:
      return st_going_in_circles::makeFMainSpec(stateId);
    case ST_MILLENNIUM_BUG:
      return st_millennium_bug::makeFMainSpec(stateId);
    case ST_BACK_TO_NORMAL:
      return st_back_to_normal::makeFMainSpec(stateId);

    case ST_TEST:
      return st_test::makeFMainSpec(stateId);
    default:
      EXCEPTION("Unrecognised state id");
  }
}
