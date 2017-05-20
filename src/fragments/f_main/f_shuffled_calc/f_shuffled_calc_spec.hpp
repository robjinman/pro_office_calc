#ifndef __PROCALC_FRAGMENTS_F_SHUFFLED_CALC_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_SHUFFLED_CALC_SPEC_HPP__


#include <string>
#include <QColor>
#include "fragment_spec.hpp"


struct FShuffledCalcSpec : public FragmentSpec {
  FShuffledCalcSpec()
    : FragmentSpec("FShuffledCalc", {}) {}

  std::string targetValue = "";
  QColor displayColour = QColor(255, 255, 255);
  QString symbols = "1234567890.+-/*=C";
};


#endif
