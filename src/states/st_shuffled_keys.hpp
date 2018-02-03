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
  mainSpec->bgColour = QColor(180, 180, 180);
  mainSpec->glitchSpec.setEnabled(true);
  mainSpec->glitchSpec.glitchFreqMax = 10.0;
  mainSpec->shuffledCalcSpec.setEnabled(true);
  mainSpec->shuffledCalcSpec.targetValue = targetValue.toStdString();
  mainSpec->shuffledCalcSpec.displayColour = QColor(200, 200, 180);
  mainSpec->shuffledCalcSpec.symbols = "☉☿♀⊕♂♃♄⛢♅♆⚳⚴⚵⚶⚘⚕♇";
  mainSpec->fileLabel = "Fi͝l̨e";
  mainSpec->quitLabel = "Qui͢t";
  mainSpec->helpLabel = "H͠e͘l͢p";
  mainSpec->aboutLabel = "A͡b҉ou͞t̵";
  mainSpec->aboutDialogTitle = "A͞b̶out";
  mainSpec->aboutDialogText =
    "<div>"
    "  <img src='data/apex.png'>"
    "  <p align='center'><big>P̸ro͡ ͏Office͟ ̀Ca͘l̶cu҉l̴at͘or̛</big>"
    "  <br>V̧e̶r̷s̷i͡o̕n 9̸.͘99͠.͞9̴9͘9̨</p>"
    "  <p align='center'>" + targetValue + "</p>"
    "  <p align='center'>C͞opyri̵g͏ht ̨(c)͟ 1992 ̡A̵pe̡x ̢S͢yst̴e̡ms̀ In͝c̷. All͞ ri̛ghts ̷r͢e͠s̷erved̨.͏</p>"
    "</div>";
  mainSpec->settingsDialogSpec.setEnabled(true);
  mainSpec->settingsDialogSpec.width = 400;
  mainSpec->settingsDialogSpec.height = 300;
  mainSpec->settingsDialogSpec.backgroundImage = "data/loading.png";
  mainSpec->settingsDialogSpec.glitchSpec.setEnabled(true);
  mainSpec->settingsDialogSpec.loadingScreenSpec.setEnabled(true);

  return mainSpec;
}


}


#endif
