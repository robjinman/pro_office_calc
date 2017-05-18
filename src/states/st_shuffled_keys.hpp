#ifndef __PROCALC_STATES_ST_SHUFFLED_KEYS_HPP__
#define __PROCALC_STATES_ST_SHUFFLED_KEYS_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_shuffled_keys {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->calculatorSpec.setEnabled(true);

  return mainSpec;
}


}


#endif
