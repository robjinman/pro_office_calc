#ifndef __PROCALC_FRAGMENTS_F_SETUP_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_SETUP_DIALOG_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"


struct FSetupDialogSpec : public FragmentSpec {
  FSetupDialogSpec()
    : FragmentSpec("FSetupDialog", {
        &glitchSpec
      }) {}

  FGlitchSpec glitchSpec;

  QString titleText = "Setup";
  int width = 640;
  int height = 480;
};


#endif
