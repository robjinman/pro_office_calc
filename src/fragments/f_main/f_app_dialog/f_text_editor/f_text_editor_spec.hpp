#ifndef __PROCALC_FRAGMENTS_F_TEXT_EDITOR_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_TEXT_EDITOR_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"


struct FTextEditorSpec : public FragmentSpec {
  FTextEditorSpec()
    : FragmentSpec("FTextEditor", {
        &glitchSpec
      }) {}

  FGlitchSpec glitchSpec;

  QString content;
};


#endif
