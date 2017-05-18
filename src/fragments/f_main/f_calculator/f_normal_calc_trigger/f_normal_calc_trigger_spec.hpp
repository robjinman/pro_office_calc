#ifndef __PROCALC_FRAGMENTS_F_NORMAL_CALC_TRIGGER_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_NORMAL_CALC_TRIGGER_SPEC_HPP__


#include "fragment_spec.hpp"


struct FNormalCalcTriggerSpec : public FragmentSpec {
  FNormalCalcTriggerSpec()
    : FragmentSpec("FNormalCalcTrigger", {}) {}

  int stateId;
};


#endif
