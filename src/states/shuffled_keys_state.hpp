#ifndef __PROCALC_STATES_SHUFFLED_KEYS_STATE_HPP__
#define __PROCALC_STATES_SHUFFLED_KEYS_STATE_HPP__


#include "root_spec.hpp"


namespace shuffled_keys {


RootSpec* makeRootSpec() {
  RootSpec* rootSpec = new RootSpec;
  rootSpec->mainFragmentSpec.showCalculatorFragment = true;
  rootSpec->mainFragmentSpec.calculatorFragmentSpec.fragment =
    CalculatorFragmentSpec::FRAG_SHUFFLED_KEYS;
  rootSpec->mainFragmentSpec.calculatorFragmentSpec.bgColour = QColor(100, 140, 250);

  return rootSpec;
}


}


#endif
