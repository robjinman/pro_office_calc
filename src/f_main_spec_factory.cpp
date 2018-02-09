#include "f_main_spec_factory.hpp"
#include "exception.hpp"
#include "state_ids.hpp"
#include "states/st_normal_calc.hpp"
#include "states/st_danger_infinity.hpp"
#include "states/st_shuffled_keys.hpp"
#include "states/st_are_you_sure.hpp"
#include "states/st_its_raining_tetrominos.hpp"
#include "states/st_office_assistant.hpp"
#include "states/st_youve_got_mail.hpp"
#include "states/st_suicide_mission.hpp"
#include "states/st_limbo.hpp"
#include "states/st_a_simple_favour.hpp"
#include "states/st_clippys_revenge.hpp"
#include "states/st_t_minus_2_minutes.hpp"
#include "states/st_back_to_normal.hpp"
#include "states/st_raycast.hpp" // TODO: Remove
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
    case ST_ARE_YOU_SURE:
      return st_are_you_sure::makeFMainSpec(stateId);
    case ST_ITS_RAINING_TETROMINOS:
      return st_its_raining_tetrominos::makeFMainSpec(stateId);
    case ST_OFFICE_ASSISTANT:
      return st_office_assistant::makeFMainSpec(stateId);
    case ST_YOUVE_GOT_MAIL:
      return st_youve_got_mail::makeFMainSpec(stateId);
    case ST_SUICIDE_MISSION:
      return st_suicide_mission::makeFMainSpec(stateId);
    case ST_LIMBO:
      return st_limbo::makeFMainSpec(stateId);
    case ST_A_SIMPLE_FAVOUR:
      return st_a_simple_favour::makeFMainSpec(stateId);
    case ST_CLIPPYS_REVENGE:
      return st_clippys_revenge::makeFMainSpec(stateId);
    case ST_T_MINUS_2_MINUTES:
      return st_t_minus_2_minutes::makeFMainSpec(stateId);
    case ST_BACK_TO_NORMAL:
      return st_back_to_normal::makeFMainSpec(stateId);

    case ST_RAYCAST: // TODO: Remove
      return st_raycast::makeFMainSpec(stateId);
    default:
      EXCEPTION("Unrecognised state id");
  }
}
