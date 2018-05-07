#ifndef __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"
#include "fragments/f_main/f_settings_dialog/f_loading_screen/f_loading_screen_spec.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze_spec.hpp"


struct FSettingsDialogSpec : public FragmentSpec {
  FSettingsDialogSpec()
    : FragmentSpec("FSettingsDialog", {
        &glitchSpec,
        &loadingScreenSpec,
        &configMazeSpec
      }) {}

  FGlitchSpec glitchSpec;
  FLoadingScreenSpec loadingScreenSpec;
  FConfigMazeSpec configMazeSpec;

  QString titleText = "Settings";
  int width = 400;
  int height = 300;
  QString backgroundImage;
};


#endif
