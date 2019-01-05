#ifndef __PROCALC_STATES_ST_GOING_IN_CIRCLES_HPP__
#define __PROCALC_STATES_ST_GOING_IN_CIRCLES_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_going_in_circles {


FMainSpec* makeFMainSpec(const AppConfig& appConfig) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->windowTitle = "Pro O҉f̶fic͡e Calc͠u͜l̡ator͏";
  mainSpec->glitchSpec.setEnabled(true);
  mainSpec->fileLabel = "Fi͝l̨e";
  mainSpec->quitLabel = "Qui͢t";
  mainSpec->backgroundImage = appConfig.dataPath("common/images/bliss.png").c_str();
  mainSpec->helpLabel = "H͠e͘l͢p";
  mainSpec->aboutLabel = "A͡b҉ou͞t̵";
  mainSpec->aboutDialogTitle = "A͞b̶out";
  mainSpec->aboutDialogText = QString() +
    "<div>"
    "  <img src='" + appConfig.dataPath("common/images/apex.png").c_str() + "'>"
    "  <p align='center'><big>P̸ro͡ ͏Office͟ ̀Ca͘l̶cu҉l̴at͘or̛</big>"
    "  <br>V̧e̶r̷s̷i͡o̕n " + appConfig.version.c_str() + "</p>"
    "  <p align='center'>C͞opyri̵g͏ht ̨(c)͟ 1997 ̡A̵pe̡x ̢S͢yst̴e̡ms̀ In͝c̷. All͞ ri̛ghts ̷r͢e͠s̷erved̨.͏</p>"
    "</div>";
  mainSpec->desktopSpec.setEnabled(true);
  mainSpec->desktopSpec.icons = {
    {appConfig.dataPath("going_in_circles/file_browser.png"), "File Browser", "fileBrowserLaunch"},
  };
  mainSpec->appDialogSpec0.setEnabled(true);
  mainSpec->appDialogSpec0.titleText = "File Browser";
  mainSpec->appDialogSpec0.width = 400;
  mainSpec->appDialogSpec0.height = 300;
  mainSpec->appDialogSpec0.showOnEvent = "fileBrowserLaunch";
  mainSpec->appDialogSpec0.fileSystemSpec.setEnabled(true);

  return mainSpec;
}


}


#endif
