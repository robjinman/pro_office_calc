#include "fragment_factory.hpp"
#include "exception.hpp"
#include "utils.hpp"
#include "fragments/relocatable/f_glitch/f_glitch.hpp"
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_about_dialog/f_about_dialog.hpp"
#include "fragments/f_main/f_countdown_to_start/f_countdown_to_start.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_loading_screen/f_loading_screen.hpp"
#include "fragments/f_main/f_settings_dialog/f_maze_3d/f_maze_3d.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_preferences_dialog/f_preferences_dialog.hpp"
#include "fragments/f_main/f_calculator/f_calculator.hpp"
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger.hpp"
#include "fragments/f_main/f_shuffled_calc/f_shuffled_calc.hpp"
#include "fragments/f_main/f_tetrominos/f_tetrominos.hpp"


using std::string;


//===========================================
// constructFragment
//===========================================
Fragment* constructFragment(const string& name, Fragment& parent, FragmentData& parentData) {
  DBG_PRINT("constructFragment(), name=" << name << "\n");

  if (name == "FGlitch") {
    return new FGlitch(parent, parentData);
  }
  else if (name == "FAboutDialog") {
    return new FAboutDialog(parent, parentData);
  }
  else if (name == "FCalculator") {
    return new FCalculator(parent, parentData);
  }
  else if (name == "FNormalCalcTrigger") {
    return new FNormalCalcTrigger(parent, parentData);
  }
  else if (name == "FShuffledCalc") {
    return new FShuffledCalc(parent, parentData);
  }
  else if (name == "FCountdownToStart") {
    return new FCountdownToStart(parent, parentData);
  }
  else if (name == "FSettingsDialog") {
    return new FSettingsDialog(parent, parentData);
  }
  else if (name == "FLoadingScreen") {
    return new FLoadingScreen(parent, parentData);
  }
  else if (name == "FLoginScreen") {
    return new FLoginScreen(parent, parentData);
  }
  else if (name == "FMaze3d") {
    return new FMaze3d(parent, parentData);
  }
  else if (name == "FPreferencesDialog") {
    return new FPreferencesDialog(parent, parentData);
  }
  else if (name == "FConfigMaze") {
    return new FConfigMaze(parent, parentData);
  }
  else if (name == "FTetrominos") {
    return new FTetrominos(parent, parentData);
  }

  EXCEPTION("Cannot construct fragment with unrecognised name '" << name << "'\n");
}
