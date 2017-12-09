#ifndef __PROCALC_FRAGMENTS_F_GLITCH_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_GLITCH_SPEC_HPP__


#include "fragment_spec.hpp"


struct FGlitchSpec : public FragmentSpec {
  FGlitchSpec()
    : FragmentSpec("FGlitch", {}) {}

  double glitchFreqMin = 0.1;
  double glitchFreqMax = 2.0;
  double glitchDuration = 0.1;
};


#endif
