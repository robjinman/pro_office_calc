#ifndef __PROCALC_FRAGMENTS_F_MENU_BAR_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_MENU_BAR_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog_spec.hpp"


struct FMenuBarSpec : public FragmentSpec {
  FMenuBarSpec()
    : FragmentSpec("FMenuBar", {
        &settingsDialogSpec
      }) {}

  FSettingsDialogSpec settingsDialogSpec;

  QString fileLabel = "File";
  QString quitLabel = "Quit";
  QString helpLabel = "Help";
  QString aboutLabel = "About";
  QString aboutDialogTitle = "About";
  QString aboutDialogText;
};


#endif
