#ifndef __PROCALC_STATES_ST_SHUFFLED_KEYS_HPP__
#define __PROCALC_STATES_ST_SHUFFLED_KEYS_HPP__


#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "fragments/f_main/f_main_spec.hpp"


namespace st_shuffled_keys {


FMainSpec* makeFMainSpec(int stateId) {
  srand(time(nullptr));

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(7) << std::fixed << std::setprecision(2)
    << static_cast<double>(rand() % 1000000) * 0.01;

  QString targetValue(ss.str().c_str());

  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->windowTitle = "Pro O҉f̶fic͡e Calc͠u͜l̡ator͏";
  mainSpec->bgColour = QColor(160, 160, 160);
  mainSpec->shuffledCalcSpec.setEnabled(true);
  mainSpec->shuffledCalcSpec.targetValue = targetValue.toStdString();
  mainSpec->shuffledCalcSpec.displayColour = QColor(160, 120, 120);
  mainSpec->shuffledCalcSpec.symbols = "☀☯⚙⚐⚶☂⚥☘♛♬⚒⚕⚽☠⚓♞⚖";
  mainSpec->menuBarSpec.setEnabled(true);
  mainSpec->menuBarSpec.fileLabel = "Fi͝l̨e";
  mainSpec->menuBarSpec.quitLabel = "Qui͢t";
  mainSpec->menuBarSpec.helpLabel = "H͠e͘l͢p";
  mainSpec->menuBarSpec.aboutLabel = "A͡b҉ou͞t̵";
  mainSpec->menuBarSpec.aboutDialogTitle = "A͞b̶out";
  mainSpec->menuBarSpec.aboutDialogText =
    "<div>"
    "  <img src='data/apex.png'>"
    "  <p align='center'><big>P̸ro͡ ͏Office͟ ̀Ca͘l̶cu҉l̴at͘or̛</big>"
    "  <br>V̧e̶r̷s̷i͡o̕n 9̸.͘99͠.͞9̴9͘9̨</p>"
    "  <p align='center'>" + targetValue + "</p>"
    "  <p align='center'>C͞opyri̵g͏ht ̨(c)͟ 1992 ̡A̵pe̡x ̢S͢yst̴e̡ms̀ In͝c̷. All͞ ri̛ghts ̷r͢e͠s̷erved̨.͏</p>"
    "</div>";
  mainSpec->menuBarSpec.settingsDialogSpec.setEnabled(true);
  mainSpec->menuBarSpec.settingsDialogSpec.width = 640;
  mainSpec->menuBarSpec.settingsDialogSpec.height = 480;
  mainSpec->menuBarSpec.settingsDialogSpec.loadingScreenSpec.setEnabled(true);
  mainSpec->menuBarSpec.settingsDialogSpec.loadingScreenSpec.backgroundImage = "data/loading.png";

  return mainSpec;
}


}


#endif
