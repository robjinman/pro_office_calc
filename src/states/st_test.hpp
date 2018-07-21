#ifndef __PROCALC_STATES_ST_TEST_HPP__
#define __PROCALC_STATES_ST_TEST_HPP__


#include "fragments/f_main/f_main_spec.hpp"
#include "app_config.hpp"


namespace st_test {


FMainSpec* makeFMainSpec(const AppConfig& appConfig) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->width = 640;
  mainSpec->height = 500;
  mainSpec->windowTitle = "Pro O҉f̶fic͡e Calc͠u͜l̡ator͏";
  mainSpec->fileLabel = "Fi͝l̨e";
  mainSpec->quitLabel = "Qui͢t";
  mainSpec->helpLabel = "H͠e͘l͢p";
  mainSpec->aboutLabel = "A͡b҉ou͞t̵";
  mainSpec->aboutDialogTitle = "A͞b̶out";
  mainSpec->aboutDialogText = QString() +
    "<div>"
    "  <img src='" + appConfig.dataPath("common/images/apex.png").c_str() + "'>"
    "  <p align='center'><big>P̸ro͡ ͏Office͟ ̀Ca͘l̶cu҉l̴at͘or̛</big>"
    "  <br>V̧e̶r̷s̷i͡o̕n 9̸.͘99͠.͞9̴9͘9̨</p>"
    "  <p align='center'>C͞opyri̵g͏ht ̨(c)͟ 1992 ̡A̵pe̡x ̢S͢yst̴e̡ms̀ In͝c̷. All͞ ri̛ghts ̷r͢e͠s̷erved̨.͏</p>"
    "</div>";
  mainSpec->maze3dSpec.setEnabled(true);
  mainSpec->maze3dSpec.mapFile = appConfig.dataPath("common/maps/test.svg");
  mainSpec->maze3dSpec.width = 640;
  mainSpec->maze3dSpec.height = 480;
  mainSpec->maze3dSpec.frameRate = 200;

  return mainSpec;
}


}


#endif
