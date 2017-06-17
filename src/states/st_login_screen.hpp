#ifndef __PROCALC_STATES_ST_LOGIN_SCREEN_HPP__
#define __PROCALC_STATES_ST_LOGIN_SCREEN_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_login_screen {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->windowTitle = "P̵͍̖̅͋r̵̢͓͒o̵͂̋ͅ ̶̩̝͐͒O̶̟̥͌f̵̢͙͑f̶̥̩̂͆i̷̻͐c̷̳̅̃e̴̢̞̓ ̴͓͂́C̸̻̳͗a̵̱̙̓l̶̯̬̉͘ć̶͎͚͠ụ̸̢̊͐l̸̦̀͐a̶̻͝t̵͇̂o̵͙̤̓̿r̷̛͖͝";
  mainSpec->bgColour = QColor(160, 160, 160);
  mainSpec->menuBarSpec.setEnabled(true);
  mainSpec->menuBarSpec.fileLabel = "F̶̱͍̏͂ḭ̷̺̉͐l̷̦̹̃͛ẽ̶͈̳";
  mainSpec->menuBarSpec.quitLabel = "Q̵͎̠̊͘u̴̖͙͝͝i̴̯̯͛̈́ţ̴͗";
  mainSpec->menuBarSpec.helpLabel = "H̶̳̐͌e̴̪̓l̶̡̡̋̈́p̴̠͜͝";
  mainSpec->menuBarSpec.aboutLabel = "A̷̮͆̚b̵̞̰̊̿o̶̼͓̊u̸͓͊t̵̲̭͒";
  mainSpec->menuBarSpec.aboutDialogTitle = "Â̸͍͍̈́b̶̭̬͒́õ̸̲̝u̸͎̬̒̋ť̶̪";
  mainSpec->menuBarSpec.aboutDialogText = "";
  mainSpec->menuBarSpec.aboutDialogText += "<p align='center'><big>P̶̤̻̿́r̷̬̓o̴͆̄ͅ ̵͖̞̇Ǒ̷͎͌f̵̛̺̿f̶͎̯̽̐i̸̛͉͠c̴͓͆͌e̸̩̼̽ ̷̳̓͛C̶̩̔̂ä̷̺̋l̴̮̝̂̓c̷͔͛͆ͅǘ̴̬̰̏l̶̳̲̓͊a̶͓̳͑̚t̴̰̣̃o̵̘͝͝r̸̢̝͆</big>"
    "<br>V̵̿̄͜e̸̪̗͆ŕ̸̢̻s̷̩̙͆i̸̳͊o̴̖͚̐ǹ̴̝͉ ̴̧̖̽͂1̵̲͚͗.̶̩̊̓0̸̫͓̽.̷͕͊0̶͎̝̇</p>"
    "<p align='center'></p>"
    "<p align='center'>C̸̡̖̔͂o̴̡͒̊p̷̼̏̆y̵̥͛r̶̭̘͑̋i̷̡͊͌ḡ̴̜h̴̖̚ẗ̷̯̳́ ̸̨͙̿(̷̹̯͗ć̸͔͋)̵̮̘͐͒ ̵̦̞͐̀2̷̩̫̈́͆0̷͍͔̀͑1̵̹͖̅7̵̤̟̽̍ ̷͓̐A̵̮̖̋c̷̞̾͘m̸̬͉̃é̴̡͐ ̷̮̟̄Ỉ̸̗̤̃n̵̦̊̃č̵̮̔ͅ.̴͇̙̀ ̴͇̚A̵̛̞̐ͅl̴͎̈́͘l̸̘̓͋ ̴̪͔̎r̸̩̠̐̄i̷̝͒g̶̡̦̍h̸̥̓̍ṯ̵̻͊̓s̷̰͂ ̵̘͐̊͜r̵̺̭̃e̷̱͇̔̚s̸͇͗̅e̴̜͝r̶̲̊̋v̵̧̦̈́̔e̷̤̭͑ḑ̷̉.̸̺̓͜</p>";
  mainSpec->menuBarSpec.settingsDialogSpec.setEnabled(true);
  mainSpec->menuBarSpec.settingsDialogSpec.width = 480;
  mainSpec->menuBarSpec.settingsDialogSpec.height = 360;
  mainSpec->menuBarSpec.settingsDialogSpec.raycastSpec.setEnabled(true);

  return mainSpec;
}


}


#endif
