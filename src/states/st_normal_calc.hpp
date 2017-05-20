#ifndef __PROCALC_STATES_ST_NORMAL_CALC_HPP__
#define __PROCALC_STATES_ST_NORMAL_CALC_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_normal_calc {


static void makeMenuBarSpec(FMenuBarSpec& spec, int stateId) {
  int count = 10 - stateId;

  QString clue1;
  clue1 += "<i>" + QString::number(count) + "</i>";
  QString clue2 = "<font size=6>⚠∞</font>";

  spec.setEnabled(true);
  spec.aboutDialogText = "";
  spec.aboutDialogText += "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
    "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
    "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
    + (count > 0 ? clue1 : clue2);
}


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->calculatorSpec.setEnabled(true);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.setEnabled(true);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.stateId = stateId;
  makeMenuBarSpec(mainSpec->menuBarSpec, stateId);
  mainSpec->countdownToStartSpec.setEnabled(true);
  mainSpec->countdownToStartSpec.stateId = stateId;

  return mainSpec;
}


}


#endif
