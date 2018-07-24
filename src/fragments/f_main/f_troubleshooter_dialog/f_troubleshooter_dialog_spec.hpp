#ifndef __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_tetrominos/f_tetrominos_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"


struct FTroubleshooterDialogSpec : public FragmentSpec {
  FTroubleshooterDialogSpec()
    : FragmentSpec("FTroubleshooterDialog", {
        &glitchSpec,
        &tetrominosSpec
      }) {}

  FGlitchSpec glitchSpec;
  FTetrominosSpec tetrominosSpec;

  QString titleText = "Troubleshooter";
};


#endif
