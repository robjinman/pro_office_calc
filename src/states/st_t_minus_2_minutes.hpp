#ifndef __PROCALC_STATES_ST_T_MINUS_2_MINUTES_HPP__
#define __PROCALC_STATES_ST_T_MINUS_2_MINUTES_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_t_minus_2_minutes {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->calculatorSpec.setEnabled(true);
  mainSpec->aboutDialogText = "";
  mainSpec->aboutDialogText += "<p align='center'><big>Pro Office Calculator</big>"
    "<br>Version 1.0.0</p>"
    "<p align='center'>Copyright (c) 2017 Rob Jinman. All rights reserved.</p>"
    "<i>" + QString::number(10 - stateId) + "</i>";
  mainSpec->countdownToStartSpec.setEnabled(true);
  mainSpec->countdownToStartSpec.stateId = stateId;

  return mainSpec;
}


}


#endif