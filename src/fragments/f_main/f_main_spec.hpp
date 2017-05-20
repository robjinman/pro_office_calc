#ifndef __PROCALC_FRAGMENTS_F_MAIN_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_SPEC_HPP__


#include <QColor>
#include <QString>
#include "fragment_spec.hpp"
#include "fragments/f_main/f_calculator/f_calculator_spec.hpp"
#include "fragments/f_main/f_shuffled_calc/f_shuffled_calc_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_menu_bar_spec.hpp"
#include "fragments/f_main/f_countdown_to_start/f_countdown_to_start_spec.hpp"


struct FMainSpec : public FragmentSpec {
  FMainSpec()
    : FragmentSpec("FMain", {
        &calculatorSpec,
        &shuffledCalcSpec,
        &menuBarSpec,
        &countdownToStartSpec
      }) {}

  FCalculatorSpec calculatorSpec;
  FShuffledCalcSpec shuffledCalcSpec;
  FMenuBarSpec menuBarSpec;
  FCountdownToStartSpec countdownToStartSpec;

  QString windowTitle = "Pro Office Calculator";
  QColor bgColour = QColor(240, 240, 240);
};


#endif
