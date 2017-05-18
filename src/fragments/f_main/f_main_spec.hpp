#ifndef __PROCALC_FRAGMENTS_F_MAIN_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_SPEC_HPP__


#include "fragment_spec.hpp"
#include "fragments/f_main/f_calculator/f_calculator_spec.hpp"
#include "fragments/f_main/f_normal_calc_about_dialog/f_normal_calc_about_dialog_spec.hpp"


struct FMainSpec : public FragmentSpec {
  FMainSpec()
    : FragmentSpec("FMain", {
        &calculatorSpec,
        &normalCalcAboutDialogSpec
      }) {}

  FCalculatorSpec calculatorSpec;
  FNormalCalcAboutDialogSpec normalCalcAboutDialogSpec;
};


#endif
