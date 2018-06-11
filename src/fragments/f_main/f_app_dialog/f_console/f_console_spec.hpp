#ifndef __PROCALC_FRAGMENTS_F_CONSOLE_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_CONSOLE_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"


struct FConsoleSpec : public FragmentSpec {
  FConsoleSpec()
    : FragmentSpec("FConsole", {
        &glitchSpec
      }) {}

  FGlitchSpec glitchSpec;

  QString content;
};


#endif
