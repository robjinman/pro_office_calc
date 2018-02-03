#ifndef __PROCALC_STATES_ST_LOGIN_SCREEN_HPP__
#define __PROCALC_STATES_ST_LOGIN_SCREEN_HPP__


#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "fragments/f_main/f_main_spec.hpp"


namespace st_login_screen {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->windowTitle = "Pro O҉f̶fic͡e Calc͠u͜l̡ator͏";
  mainSpec->glitchSpec.setEnabled(true);
  mainSpec->fileLabel = "Fi͝l̨e";
  mainSpec->quitLabel = "Qui͢t";
  mainSpec->backgroundImage = "data/login.png";
  mainSpec->helpLabel = "H͠e͘l͢p";
  mainSpec->aboutLabel = "A͡b҉ou͞t̵";
  mainSpec->aboutDialogTitle = "A͞b̶out";
  mainSpec->aboutDialogText =
    "<div>"
    "  <img src='data/apex.png'>"
    "  <p align='center'><big>P̸ro͡ ͏Office͟ ̀Ca͘l̶cu҉l̴at͘or̛</big>"
    "  <br>V̧e̶r̷s̷i͡o̕n 9̸.͘99͠.͞9̴9͘9̨</p>"
    "  <p align='center'>C͞opyri̵g͏ht ̨(c)͟ 1993 ̡A̵pe̡x ̢S͢yst̴e̡ms̀ In͝c̷. All͞ ri̛ghts ̷r͢e͠s̷erved̨.͏</p>"
    "</div>";
  mainSpec->loginScreenSpec.setEnabled(true);
  mainSpec->settingsDialogSpec.setEnabled(true);
  mainSpec->settingsDialogSpec.width = 400;
  mainSpec->settingsDialogSpec.height = 300;
  mainSpec->settingsDialogSpec.configMazeSpec.setEnabled(true);
  mainSpec->settingsDialogSpec.configMazeSpec.symbols = "☉☿♀⊕♂♃♄⛢♅♆⚳⚴⚵⚶⚘⚕♇";

  return mainSpec;
}


}


#endif
