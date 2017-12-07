#ifndef __PROCALC_FRAGMENTS_F_LOGIN_SCREEN_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_LOGIN_SCREEN_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"


struct FLoginScreenSpec : public FragmentSpec {
  FLoginScreenSpec()
    : FragmentSpec("FLoginScreen", {}) {}

  QString backgroundImage;
};


#endif
