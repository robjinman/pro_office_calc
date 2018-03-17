#ifndef __PROCALC_STATES_ST_YOUVE_GOT_MAIL_HPP__
#define __PROCALC_STATES_ST_YOUVE_GOT_MAIL_HPP__


#include "fragments/f_main/f_main_spec.hpp"


namespace st_youve_got_mail {


FMainSpec* makeFMainSpec(int stateId) {
  FMainSpec* mainSpec = new FMainSpec;
  mainSpec->windowTitle = "Pro O҉f̶fic͡e Calc͠u͜l̡ator͏";
  mainSpec->glitchSpec.setEnabled(true);
  mainSpec->fileLabel = "Fi͝l̨e";
  mainSpec->quitLabel = "Qui͢t";
  mainSpec->backgroundImage = "data/bliss.png";
  mainSpec->helpLabel = "H͠e͘l͢p";
  mainSpec->aboutLabel = "A͡b҉ou͞t̵";
  mainSpec->aboutDialogTitle = "A͞b̶out";
  mainSpec->aboutDialogText = "";
  mainSpec->aboutDialogText =
    "<div>"
    "  <img src='data/apex.png'>"
    "  <p align='center'><big>P̸ro͡ ͏Office͟ ̀Ca͘l̶cu҉l̴at͘or̛</big>"
    "  <br>V̧e̶r̷s̷i͡o̕n 9̸.͘99͠.͞9̴9͘9̨</p>"
    "  <p align='center'>C͞opyri̵g͏ht ̨(c)͟ 1992 ̡A̵pe̡x ̢S͢yst̴e̡ms̀ In͝c̷. All͞ ri̛ghts ̷r͢e͠s̷erved̨.͏</p>"
    "</div>";
  mainSpec->desktopSpec.setEnabled(true);
  mainSpec->desktopSpec.icons = {
    {"data/procalc.png", "Pro Office Calculator", "procalcLaunch"},
    {"data/youve_got_mail/text_file.png", "y2k_threat.txt", "y2kThreatLaunch"},
    {"data/youve_got_mail/mail.png", "Email Client", "mailClientLaunch"}
  };
  mainSpec->desktopSpec.serverRoomInitSpec.setEnabled(true);
  mainSpec->appDialogSpec0.setEnabled(true);
  mainSpec->appDialogSpec0.titleText = "Pro Office Calculator";
  mainSpec->appDialogSpec0.width = 400;
  mainSpec->appDialogSpec0.height = 300;
  mainSpec->appDialogSpec0.showOnEvent = "procalcLaunch";
  mainSpec->appDialogSpec0.calculatorSpec.setEnabled(true);
  mainSpec->appDialogSpec0.serverRoomSpec.setEnabled(true);
  mainSpec->appDialogSpec1.setEnabled(true);
  mainSpec->appDialogSpec1.titleText = "Mail Client";
  mainSpec->appDialogSpec1.width = 500;
  mainSpec->appDialogSpec1.height = 375;
  mainSpec->appDialogSpec1.showOnEvent = "mailClientLaunch";
  mainSpec->appDialogSpec1.mailClientSpec.setEnabled(true);
  mainSpec->appDialogSpec1.glitchSpec.setEnabled(true);
  mainSpec->appDialogSpec2.setEnabled(true);
  mainSpec->appDialogSpec2.titleText = "y2k_threat.doc";
  mainSpec->appDialogSpec2.width = 400;
  mainSpec->appDialogSpec2.height = 300;
  mainSpec->appDialogSpec2.showOnEvent = "y2kThreatLaunch";
  mainSpec->appDialogSpec2.textEditorSpec.setEnabled(true);
  mainSpec->appDialogSpec2.textEditorSpec.content = "<h2>The Y2K threat</h2>"
    "There is growing concern amongst a minority of academics and industry experts whether the "
    "various systems in use today can cope with their clocks transitioning from this century into "
    "the next - that is, from 11:59:59 pm on 31st December 1999 to 12:00:00 am on 1st January "
    "2000. Whilst there's little consensus on what if any deleterious effects may result, "
    "developers and administrators are urged to consider how their systems may be affected and to "
    "implement any precautionary measures well in advance.";

  return mainSpec;
}


}


#endif
