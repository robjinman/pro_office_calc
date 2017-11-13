#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_loading_screen/f_loading_screen.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_loading_screen/f_loading_screen_spec.hpp"


//===========================================
// FLoadingScreen::FLoadingScreen
//===========================================
FLoadingScreen::FLoadingScreen(Fragment& parent_, FragmentData& parentData_)
  : QLabel(nullptr),
    Fragment("FLoadingScreen", parent_, parentData_, m_data) {}

//===========================================
// FLoadingScreen::rebuild
//===========================================
void FLoadingScreen::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FLoadingScreenSpec&>(spec_);

  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  QPixmap tmp(spec.backgroundImage);
  m_data.background.reset(new QPixmap(tmp.scaledToHeight(parent.size().height())));

  setPixmap(*m_data.background);

  parentData.vbox->setSpacing(0);
  parentData.vbox->setContentsMargins(0, 0, 0, 0);
  parentData.vbox->addWidget(this);

  Fragment::rebuild(spec_);
}

//===========================================
// FLoadingScreen::cleanUp
//===========================================
void FLoadingScreen::cleanUp() {
  // TODO
}
