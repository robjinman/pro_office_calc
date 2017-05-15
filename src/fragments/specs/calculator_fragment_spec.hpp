#ifndef __PROCALC_FRAGMENTS_SPECS_CALCULATOR_FRAGMENT_SPEC_HPP__
#define __PROCALC_FRAGMENTS_SPECS_CALCULATOR_FRAGMENT_SPEC_HPP__


#include <QColor>
#include "fragments/specs/normal_calculator_fragment_spec.hpp"


struct CalculatorFragmentSpec {
  enum { FRAG_NORMAL_CALC, FRAG_SHUFFLED_KEYS } fragment;
  NormalCalculatorFragmentSpec normalCalculatorFragmentSpec;

  QColor bgColour;
};


#endif
