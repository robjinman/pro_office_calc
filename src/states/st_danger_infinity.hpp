#ifndef __PROCALC_STATES_ST_DANGER_INFINITY_HPP__
#define __PROCALC_STATES_ST_DANGER_INFINITY_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_danger_infinity {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->calculatorSpec.setEnabled(true);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.setEnabled(true);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.targetWindowColour = QColor(160, 160, 160);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.targetDisplayColour = QColor(160, 120, 120);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.symbols = "☀☯⚙⚐⚶☂⚥☘♛♬⚒⚕⚽☠⚓♞⚖";
  mainSpec->menuBarSpec.setEnabled(true);
  mainSpec->menuBarSpec.aboutDialogText =
    "<div>"
    "  <img src='data/apex.svg'>"
    "  <p align='center'><big>Pro Office Calculator</big>"
    "  <br>Version 1.0.0</p>"
    "  <p align='center'>Copyright (c) 2017 Apex Systems Inc. All rights reserved.</p>"
    "  <p><font size=6>⚠∞</font></p>"
    "</div>";

  return mainSpec;
}


}


#endif
