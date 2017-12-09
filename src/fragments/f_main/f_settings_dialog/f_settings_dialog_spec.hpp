#ifndef __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"
#include "fragments/f_main/f_settings_dialog/f_loading_screen/f_loading_screen_spec.hpp"
#include "fragments/f_main/f_settings_dialog/f_login_screen/f_login_screen_spec.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/f_raycast_spec.hpp"


struct FSettingsDialogSpec : public FragmentSpec {
  FSettingsDialogSpec()
    : FragmentSpec("FSettingsDialog", {
        &glitchSpec,
        &loadingScreenSpec,
        &loginScreenSpec,
        &raycastSpec
      }) {}

  FGlitchSpec glitchSpec;
  FLoadingScreenSpec loadingScreenSpec;
  FLoginScreenSpec loginScreenSpec;
  FRaycastSpec raycastSpec;

  QString titleText = "Settings";
  int width = 640;
  int height = 480;
};


#endif