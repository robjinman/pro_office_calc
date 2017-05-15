#ifndef __PROCALC_FRAGMENTS_SPECS_MAIN_FRAGMENT_SPEC_HPP__
#define __PROCALC_FRAGMENTS_SPECS_MAIN_FRAGMENT_SPEC_HPP__


#include <QColor>
#include "fragments/specs/calculator_fragment_spec.hpp"


struct MainFragmentSpec {
  bool showCalculatorFragment = false;
  CalculatorFragmentSpec calculatorFragmentSpec;

  QColor bgColour;
};


#endif
