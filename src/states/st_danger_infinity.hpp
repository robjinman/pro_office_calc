#ifndef __PROCALC_STATES_ST_DANGER_INFINITY_HPP__
#define __PROCALC_STATES_ST_DANGER_INFINITY_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_danger_infinity {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->calculatorSpec.setEnabled(true);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.setEnabled(true);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.targetWindowColour = QColor(180, 180, 180);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.targetDisplayColour = QColor(200, 200, 180);
  mainSpec->calculatorSpec.normalCalcTriggerSpec.symbols = "☀☯⚙⚐⚶☂⚥☘♛♬⚒⚕⚽☠⚓♞⚖";
  mainSpec->glitchSpec.setEnabled(true);
  mainSpec->glitchSpec.glitchFreqMin = 10.0;
  mainSpec->glitchSpec.glitchFreqMax = 20.0;
  mainSpec->aboutDialogSpec.setEnabled(true);
  mainSpec->aboutDialogSpec.aboutDialogText += "<p align='center'><big>Pro Office Calculator</big>"
    "<br>Version 1.0.0</p>"
    "<p align='center'>Copyright (c) 2017 Rob Jinman. All rights reserved.</p>"
    "<font size=6>⚠∞</font>";

  return mainSpec;
}


}


#endif
