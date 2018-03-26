#ifndef __PROCALC_STATES_ST_MAKING_PROGRESS_HPP__
#define __PROCALC_STATES_ST_MAKING_PROGRESS_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_making_progress {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->windowTitle = "Pro O҉f̶fic͡e Calc͠u͜l̡ator͏";
  mainSpec->glitchSpec.setEnabled(true);
  mainSpec->fileLabel = "Fi͝l̨e";
  mainSpec->quitLabel = "Qui͢t";
  mainSpec->backgroundImage = "data/common/images/bliss.png";
  mainSpec->helpLabel = "H͠e͘l͢p";
  mainSpec->aboutLabel = "A͡b҉ou͞t̵";
  mainSpec->aboutDialogTitle = "A͞b̶out";
  mainSpec->aboutDialogText = "";
  mainSpec->aboutDialogText =
    "<div>"
    "  <img src='data/common/images/apex.png'>"
    "  <p align='center'><big>P̸ro͡ ͏Office͟ ̀Ca͘l̶cu҉l̴at͘or̛</big>"
    "  <br>V̧e̶r̷s̷i͡o̕n 9̸.͘99͠.͞9̴9͘9̨</p>"
    "  <p align='center'>C͞opyri̵g͏ht ̨(c)͟ 1992 ̡A̵pe̡x ̢S͢yst̴e̡ms̀ In͝c̷. All͞ ri̛ghts ̷r͢e͠s̷erved̨.͏</p>"
    "</div>";
  mainSpec->desktopSpec.setEnabled(true);
  mainSpec->desktopSpec.icons = {
    {"data/common/images/procalc.png", "Pro Office Calculator", "procalcLaunch"}
  };
  mainSpec->appDialogSpec0.setEnabled(true);
  mainSpec->appDialogSpec0.name = "procalcSetup";
  mainSpec->appDialogSpec0.titleText = "Installation Setup";
  mainSpec->appDialogSpec0.width = 320;
  mainSpec->appDialogSpec0.height = 240;
  mainSpec->appDialogSpec0.showOnEvent = "procalcLaunch";
  mainSpec->appDialogSpec0.procalcSetupSpec.setEnabled(true);
  mainSpec->appDialogSpec1.setEnabled(true);
  mainSpec->appDialogSpec1.titleText = "Pro Office Calculator";
  mainSpec->appDialogSpec1.width = 400;
  mainSpec->appDialogSpec1.height = 300;
  mainSpec->appDialogSpec1.showOnEvent = "makingProgress/setupComplete";
  mainSpec->appDialogSpec1.calculatorSpec.setEnabled(true);
  mainSpec->appDialogSpec1.calculatorSpec.partialCalcSpec.setEnabled(true);

  return mainSpec;
}


}


#endif
