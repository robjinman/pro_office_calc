#ifndef __PROCALC_FRAGMENTS_F_MAIN_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_SPEC_HPP__


#include <QColor>
#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"
#include "fragments/f_main/f_calculator/f_calculator_spec.hpp"
#include "fragments/f_main/f_shuffled_calc/f_shuffled_calc_spec.hpp"
#include "fragments/f_main/f_countdown_to_start/f_countdown_to_start_spec.hpp"
#include "fragments/f_main/f_settings_dialog/f_settings_dialog_spec.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen_spec.hpp"
#include "fragments/f_main/f_preferences_dialog/f_preferences_dialog_spec.hpp"
#include "fragments/f_main/f_tetrominos/f_tetrominos_spec.hpp"


struct FMainSpec : public FragmentSpec {
  FMainSpec()
    : FragmentSpec("FMain", {
        &glitchSpec,
        &calculatorSpec,
        &shuffledCalcSpec,
        &loginScreenSpec,
        &countdownToStartSpec,
        &settingsDialogSpec,
        &preferencesDialogSpec,
        &tetrominosSpec
      }) {}

  FGlitchSpec glitchSpec;
  FCalculatorSpec calculatorSpec;
  FShuffledCalcSpec shuffledCalcSpec;
  FLoginScreenSpec loginScreenSpec;
  FCountdownToStartSpec countdownToStartSpec;
  FSettingsDialogSpec settingsDialogSpec;
  FPreferencesDialogSpec preferencesDialogSpec;
  FTetrominosSpec tetrominosSpec;

  QString windowTitle = "Pro Office Calculator";
  int width = 400;
  int height = 300;
  QColor bgColour = QColor(240, 240, 240);
  QString backgroundImage;
  QString fileLabel = "File";
  QString quitLabel = "Quit";
  QString helpLabel = "Help";
  QString aboutLabel = "About";
  QString aboutDialogTitle = "About";
  QString aboutDialogText;
};


#endif
