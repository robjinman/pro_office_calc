#ifndef __PROCALC_STATES_ST_SHUFFLED_KEYS_HPP__
#define __PROCALC_STATES_ST_SHUFFLED_KEYS_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_shuffled_keys {


static void makeAboutDialogSpec(FAboutDialogSpec& spec) {
  spec.setEnabled(true);
  spec.text = "";
  spec.text += "<p align='center'><big>P̶̤̻̿́r̷̬̓o̴͆̄ͅ ̵͖̞̇Ǒ̷͎͌f̵̛̺̿f̶͎̯̽̐i̸̛͉͠c̴͓͆͌e̸̩̼̽ ̷̳̓͛C̶̩̔̂ä̷̺̋l̴̮̝̂̓c̷͔͛͆ͅǘ̴̬̰̏l̶̳̲̓͊a̶͓̳͑̚t̴̰̣̃o̵̘͝͝r̸̢̝͆</big><br>V̵̿̄͜e̸̪̗͆ŕ̸̢̻s̷̩̙͆i̸̳͊o̴̖͚̐ǹ̴̝͉ ̴̧̖̽͂1̵̲͚͗.̶̩̊̓0̸̫͓̽.̷͕͊0̶͎̝̇</p>"
    "<p align='center'>" + QString::number(4029) + "</p>"
    "<p align='center'>C̸̡̖̔͂o̴̡͒̊p̷̼̏̆y̵̥͛r̶̭̘͑̋i̷̡͊͌ḡ̴̜h̴̖̚ẗ̷̯̳́ ̸̨͙̿(̷̹̯͗ć̸͔͋)̵̮̘͐͒ ̵̦̞͐̀2̷̩̫̈́͆0̷͍͔̀͑1̵̹͖̅7̵̤̟̽̍ ̷͓̐A̵̮̖̋c̷̞̾͘m̸̬͉̃é̴̡͐ ̷̮̟̄Ỉ̸̗̤̃n̵̦̊̃č̵̮̔ͅ.̴͇̙̀ ̴͇̚A̵̛̞̐ͅl̴͎̈́͘l̸̘̓͋ ̴̪͔̎r̸̩̠̐̄i̷̝͒g̶̡̦̍h̸̥̓̍ṯ̵̻͊̓s̷̰͂ ̵̘͐̊͜r̵̺̭̃e̷̱͇̔̚s̸͇͗̅e̴̜͝r̶̲̊̋v̵̧̦̈́̔e̷̤̭͑ḑ̷̉.̸̺̓͜</p>";
}


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->bgColour = QColor(160, 160, 160);
  mainSpec->calculatorSpec.setEnabled(true);
  mainSpec->calculatorSpec.displayColour = QColor(160, 120, 120);
  makeAboutDialogSpec(mainSpec->aboutDialogSpec);

  return mainSpec;
}


}


#endif
