#ifndef __PROCALC_STATES_ST_NORMAL_CALC_HPP__
#define __PROCALC_STATES_ST_NORMAL_CALC_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_normal_calc {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->calculatorSpec.setEnabled(true);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.setEnabled(true);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.stateId = stateId;
  mainSpec->normalCalcAboutDialogSpec.setEnabled(true);
  mainSpec->normalCalcAboutDialogSpec.stateId = stateId;

  return mainSpec;
}


}


#endif
