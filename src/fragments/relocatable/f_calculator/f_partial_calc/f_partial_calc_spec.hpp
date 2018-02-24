#ifndef __PROCALC_FRAGMENTS_F_PARTIAL_CALC_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_PARTIAL_CALC_SPEC_HPP__


#include <QColor>
#include "fragment_spec.hpp"


struct FPartialCalcSpec : public FragmentSpec {
  FPartialCalcSpec()
    : FragmentSpec("FPartialCalc", {}) {}

  int stateId;
  QColor targetWindowColour;
  QColor targetDisplayColour;
  QString symbols = "1234567890.+-/*=C";
};


#endif
