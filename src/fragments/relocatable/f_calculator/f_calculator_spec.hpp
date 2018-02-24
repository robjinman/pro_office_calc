#ifndef __PROCALC_FRAGMENTS_F_CALCULATOR_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_CALCULATOR_SPEC_HPP__


#include <QColor>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger_spec.hpp"
#include "fragments/relocatable/f_calculator/f_partial_calc/f_partial_calc_spec.hpp"


struct FCalculatorSpec : public FragmentSpec {
  FCalculatorSpec()
    : FragmentSpec("FCalculator", {
        &normalCalcTriggerSpec,
        &partialCalcSpec
      }) {}

  FNormalCalcTriggerSpec normalCalcTriggerSpec;
  FPartialCalcSpec partialCalcSpec;

  QColor displayColour = QColor(255, 255, 255);
};


#endif
