#ifndef __PROCALC_FRAGMENTS_F_ABOUT_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_ABOUT_DIALOG_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"


struct FAboutDialogSpec : public FragmentSpec {
  FAboutDialogSpec()
    : FragmentSpec("FAboutDialog", {}) {}

  QString helpLabel = "Help";
  QString aboutLabel = "About";
  QString aboutDialogTitle = "About";
  QString aboutDialogText;
};


#endif
