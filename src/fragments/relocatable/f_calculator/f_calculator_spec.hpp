#ifndef __PROCALC_FRAGMENTS_F_CALCULATOR_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_CALCULATOR_SPEC_HPP__


#include <QColor>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger_spec.hpp"


struct FCalculatorSpec : public FragmentSpec {
  FCalculatorSpec()
    : FragmentSpec("FCalculator", {
        &normalCalcTriggerSpec,
      }) {}

  FNormalCalcTriggerSpec normalCalcTriggerSpec;

  QColor displayColour = QColor(255, 255, 255);
};


#endif
