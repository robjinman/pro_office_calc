#include "fragment_factory.hpp"
#include "exception.hpp"
#include "utils.hpp"
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_normal_calc_about_dialog/f_normal_calc_about_dialog.hpp"
#include "fragments/f_main/f_calculator/f_calculator.hpp"
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger.hpp"


using std::string;


//===========================================
// constructFragment
//===========================================
Fragment* constructFragment(const string& name, Fragment& parent, FragmentData& parentData) {
  DBG_PRINT("constructFragment(), name=" << name << "\n");

  if (name == "FNormalCalcAboutDialog") {
    return new FNormalCalcAboutDialog(parent, parentData);
  }
  else if (name == "FCalculator") {
    return new FCalculator(parent, parentData);
  }
  else if (name == "FNormalCalcTrigger") {
    return new FNormalCalcTrigger(parent, parentData);
  }

  EXCEPTION("Cannot construct fragment with unrecognised name '" << name << "'\n");
}
