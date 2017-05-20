#ifndef __PROCALC_FRAGMENTS_F_MENU_BAR_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_MENU_BAR_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"


struct FMenuBarSpec : public FragmentSpec {
  FMenuBarSpec()
    : FragmentSpec("FMenuBar", {}) {}

  QString fileLabel = "File";
  QString quitLabel = "Quit";
  QString helpLabel = "Help";
  QString aboutLabel = "About";
  QString aboutDialogTitle = "About";
  QString aboutDialogText;
};


#endif
