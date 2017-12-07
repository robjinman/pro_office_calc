#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_settings_dialog/f_login_screen/f_login_screen_spec.hpp"
#include "effects.hpp"


//===========================================
// FLoginScreen::FLoginScreen
//===========================================
FLoginScreen::FLoginScreen(Fragment& parent_, FragmentData& parentData_)
  : QLabel(nullptr),
    Fragment("FLoginScreen", parent_, parentData_, m_data) {}

//===========================================
// FLoginScreen::rebuild
//===========================================
void FLoginScreen::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  m_origParentState.spacing = parentData.vbox->spacing();
  m_origParentState.margins = parentData.vbox->contentsMargins();

  parentData.vbox->setSpacing(0);
  parentData.vbox->setContentsMargins(0, 0, 0, 0);
  parentData.vbox->addWidget(this);

  auto& spec = dynamic_cast<const FLoginScreenSpec&>(spec_);

  QPixmap tmp(spec.backgroundImage);
  m_data.background.reset(new QPixmap(tmp.scaledToHeight(parent.size().height())));

  setPixmap(*m_data.background);

  Fragment::rebuild(spec_);
}

//===========================================
// FLoginScreen::cleanUp
//===========================================
void FLoginScreen::cleanUp() {
  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->setSpacing(m_origParentState.spacing);
  parentData.vbox->setContentsMargins(m_origParentState.margins);
  parentData.vbox->removeWidget(this);
}
