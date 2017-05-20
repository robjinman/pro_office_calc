#ifndef __PROCALC_FRAGMENTS_F_COUNTDOWN_TO_START_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_COUNTDOWN_TO_START_SPEC_HPP__


#include "fragment_spec.hpp"


struct FCountdownToStartSpec : public FragmentSpec {
  FCountdownToStartSpec()
    : FragmentSpec("FCountdownToStart", {}) {}

  int stateId;
};


#endif
