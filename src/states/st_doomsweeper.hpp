#ifndef __PROCALC_STATES_ST_DOOMSWEEPER_HPP__
#define __PROCALC_STATES_ST_DOOMSWEEPER_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_doomsweeper {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->windowTitle = "Pro O҉f̶fic͡e Calc͠u͜l̡ator͏";
  mainSpec->glitchSpec.setEnabled(true);
  mainSpec->fileLabel = "Fi͝l̨e";
  mainSpec->quitLabel = "Qui͢t";
  mainSpec->backgroundImage = config::dataPath("common/images/bliss.png").c_str();
  mainSpec->helpLabel = "H͠e͘l͢p";
  mainSpec->aboutLabel = "A͡b҉ou͞t̵";
  mainSpec->aboutDialogTitle = "A͞b̶out";
  mainSpec->aboutDialogText = QString() +
    "<div>"
    "  <img src='" + config::dataPath("common/images/apex.png").c_str() + "'>"
    "  <p align='center'><big>P̸ro͡ ͏Office͟ ̀Ca͘l̶cu҉l̴at͘or̛</big>"
    "  <br>V̧e̶r̷s̷i͡o̕n 9̸.͘99͠.͞9̴9͘9̨</p>"
    "  <p align='center'>C͞opyri̵g͏ht ̨(c)͟ 1992 ̡A̵pe̡x ̢S͢yst̴e̡ms̀ In͝c̷. All͞ ri̛ghts ̷r͢e͠s̷erved̨.͏</p>"
    "</div>";
  mainSpec->desktopSpec.setEnabled(true);
  mainSpec->desktopSpec.icons = {
    {config::dataPath("doomsweeper/file_browser.png"), "File Browser", "fileBrowserLaunch"},
    {config::dataPath("doomsweeper/minesweeper.png"), "Doomsweeper", "doomsweeperLaunch"},
    {config::dataPath("doomsweeper/console.png"), "Terminal", "terminalLaunch"}
  };
  mainSpec->appDialogSpec0.setEnabled(true);
  mainSpec->appDialogSpec0.titleText = "File Browser";
  mainSpec->appDialogSpec0.width = 320;
  mainSpec->appDialogSpec0.height = 240;
  mainSpec->appDialogSpec0.showOnEvent = "fileBrowserLaunch";
  mainSpec->appDialogSpec0.kernelSpec.setEnabled(true);
  mainSpec->appDialogSpec1.setEnabled(true);
  mainSpec->appDialogSpec1.titleText = "Doomsweeper";
  mainSpec->appDialogSpec1.width = 280;
  mainSpec->appDialogSpec1.height = 240;
  mainSpec->appDialogSpec1.showOnEvent = "doomsweeperLaunch";
  mainSpec->appDialogSpec1.minesweeperSpec.setEnabled(true);
  mainSpec->appDialogSpec2.setEnabled(true);
  mainSpec->appDialogSpec2.titleText = "Terminal";
  mainSpec->appDialogSpec2.width = 320;
  mainSpec->appDialogSpec2.height = 240;
  mainSpec->appDialogSpec2.showOnEvent = "terminalLaunch";
  mainSpec->appDialogSpec2.consoleSpec.setEnabled(true);

  return mainSpec;
}


}


#endif
