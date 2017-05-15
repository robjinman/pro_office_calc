#ifndef __PROCALC_STATES_DANGER_INFINITY_STATE_HPP__
#define __PROCALC_STATES_DANGER_INFINITY_STATE_HPP__


#include "root_spec.hpp"


namespace danger_infinity {


RootSpec* makeRootSpec() {
  RootSpec* rootSpec = new RootSpec;
  rootSpec->mainFragmentSpec.showCalculatorFragment = true;
  rootSpec->mainFragmentSpec.calculatorFragmentSpec.fragment =
    CalculatorFragmentSpec::FRAG_NORMAL_CALC;

  return rootSpec;
}


}


#endif
