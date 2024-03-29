#ifndef __PROCALC_STATES_ST_NORMAL_CALC_HPP__
#define __PROCALC_STATES_ST_NORMAL_CALC_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_normal_calc {


FMainSpec* makeFMainSpec(const AppConfig& appConfig) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->calculatorSpec.setEnabled(true);
  mainSpec->aboutDialogText = "";
  mainSpec->aboutDialogText += QString() + "<p align='center'><big>Pro Office Calculator</big>"
    "<br>Version " + appConfig.version.c_str() + "</p>"
    "<p align='center'>Copyright (c) 2018 Rob Jinman. All rights reserved.</p>"
    "<i>" + QString::number(10 - appConfig.stateId) + "</i>";
  mainSpec->countdownToStartSpec.setEnabled(true);
  mainSpec->countdownToStartSpec.stateId = appConfig.stateId;

  return mainSpec;
}


}


#endif
