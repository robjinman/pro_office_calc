#ifndef __PROCALC_FRAGMENTS_F_NORMAL_CALC_TRIGGER_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_NORMAL_CALC_TRIGGER_SPEC_HPP__


#include <QColor>
#include "fragment_spec.hpp"


struct FNormalCalcTriggerSpec : public FragmentSpec {
  FNormalCalcTriggerSpec()
    : FragmentSpec("FNormalCalcTrigger", {}) {}

  int stateId;
  QColor targetWindowColour;
  QColor targetDisplayColour;
  QString symbols = "1234567890.+-/*=C";
};


#endif
