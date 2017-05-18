#ifndef __PROCALC_FRAGMENTS_F_NORMAL_CALC_ABOUT_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_NORMAL_CALC_ABOUT_DIALOG_SPEC_HPP__


#include "fragment_spec.hpp"


struct FNormalCalcAboutDialogSpec : public FragmentSpec {
  FNormalCalcAboutDialogSpec()
    : FragmentSpec("FNormalCalcAboutDialog", {}) {}

  int stateId = 0;
};


#endif
