#include "fragment_factory.hpp"
#include "exception.hpp"
#include "utils.hpp"
#include "fragments/relocatable/f_glitch/f_glitch.hpp"
#include "fragments/relocatable/f_tetrominos/f_tetrominos.hpp"
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_countdown_to_start/f_countdown_to_start.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_loading_screen/f_loading_screen.hpp"
#include "fragments/f_main/f_settings_dialog/f_maze_3d/f_maze_3d.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_troubleshooter_dialog/f_troubleshooter_dialog.hpp"
#include "fragments/f_main/f_calculator/f_calculator.hpp"
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger.hpp"
#include "fragments/f_main/f_shuffled_calc/f_shuffled_calc.hpp"


using std::string;


//===========================================
// constructFragment
//===========================================
Fragment* constructFragment(const string& name, Fragment& parent, FragmentData& parentData,
  const CommonFragData& commonData) {

  DBG_PRINT("constructFragment(), name=" << name << "\n");

  if (name == "FGlitch") {
    return new FGlitch(parent, parentData, commonData);
  }
  else if (name == "FCalculator") {
    return new FCalculator(parent, parentData, commonData);
  }
  else if (name == "FNormalCalcTrigger") {
    return new FNormalCalcTrigger(parent, parentData, commonData);
  }
  else if (name == "FShuffledCalc") {
    return new FShuffledCalc(parent, parentData, commonData);
  }
  else if (name == "FCountdownToStart") {
    return new FCountdownToStart(parent, parentData, commonData);
  }
  else if (name == "FSettingsDialog") {
    return new FSettingsDialog(parent, parentData, commonData);
  }
  else if (name == "FLoadingScreen") {
    return new FLoadingScreen(parent, parentData, commonData);
  }
  else if (name == "FLoginScreen") {
    return new FLoginScreen(parent, parentData, commonData);
  }
  else if (name == "FMaze3d") {
    return new FMaze3d(parent, parentData, commonData);
  }
  else if (name == "FTroubleshooterDialog") {
    return new FTroubleshooterDialog(parent, parentData, commonData);
  }
  else if (name == "FConfigMaze") {
    return new FConfigMaze(parent, parentData, commonData);
  }
  else if (name == "FTetrominos") {
    return new FTetrominos(parent, parentData, commonData);
  }

  EXCEPTION("Cannot construct fragment with unrecognised name '" << name << "'\n");
}
