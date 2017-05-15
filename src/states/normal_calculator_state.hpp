#ifndef __PROCALC_STATES_NORMAL_CALCULATOR_STATE_HPP__
#define __PROCALC_STATES_NORMAL_CALCULATOR_STATE_HPP__


#include "root_spec.hpp"


namespace normal_calculator {


RootSpec* makeRootSpec(int stateId) {
  RootSpec* rootSpec = new RootSpec;
  rootSpec->mainFragmentSpec.showCalculatorFragment = true;
  rootSpec->mainFragmentSpec.calculatorFragmentSpec.fragment =
    CalculatorFragmentSpec::FRAG_NORMAL_CALC;
  rootSpec->mainFragmentSpec.calculatorFragmentSpec.normalCalculatorFragmentSpec
    .count = 10 - stateId;

  return rootSpec;
}


}


#endif
