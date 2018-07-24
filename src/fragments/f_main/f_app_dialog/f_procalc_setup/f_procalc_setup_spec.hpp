#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"


struct FProcalcSetupSpec : public FragmentSpec {
  FProcalcSetupSpec()
    : FragmentSpec("FProcalcSetup", {
        &glitchSpec
      }) {}

  FGlitchSpec glitchSpec;
};


#endif
