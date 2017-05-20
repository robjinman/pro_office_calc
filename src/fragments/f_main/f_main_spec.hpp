#ifndef __PROCALC_FRAGMENTS_F_MAIN_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_SPEC_HPP__


#include <QColor>
#include "fragment_spec.hpp"
#include "fragments/f_main/f_calculator/f_calculator_spec.hpp"
#include "fragments/f_main/f_about_dialog/f_about_dialog_spec.hpp"
#include "fragments/f_main/f_countdown_to_start/f_countdown_to_start_spec.hpp"


struct FMainSpec : public FragmentSpec {
  FMainSpec()
    : FragmentSpec("FMain", {
        &calculatorSpec,
        &aboutDialogSpec,
        &countdownToStartSpec
      }) {}

  FCalculatorSpec calculatorSpec;
  FAboutDialogSpec aboutDialogSpec;
  FCountdownToStartSpec countdownToStartSpec;

  QColor bgColour = QColor(240, 240, 240);
};


#endif
