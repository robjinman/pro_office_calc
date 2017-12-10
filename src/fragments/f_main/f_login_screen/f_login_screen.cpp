#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen_spec.hpp"
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
  auto& parent = parentFrag<FMain>();
  auto& parentData = parentFragData<FMainData>();

  m_origParentState.centralWidget = parent.centralWidget();
  parent.setCentralWidget(this);

  auto& spec = dynamic_cast<const FLoginScreenSpec&>(spec_);

  QPixmap tmp(spec.backgroundImage);
  m_data.background.reset(new QPixmap(tmp.scaledToHeight(parent.size().height())));

  m_data.wgtUser.reset(new QLineEdit(this));
  m_data.wgtUser->setGeometry(205, 170, 100, 20);

  m_data.wgtPassword.reset(new QLineEdit(this));
  m_data.wgtPassword->setGeometry(205, 195, 100, 20);

  setPixmap(*m_data.background);

  Fragment::rebuild(spec_);
}

//===========================================
// FLoginScreen::cleanUp
//===========================================
void FLoginScreen::cleanUp() {
  auto& parent = parentFrag<FMain>();

  parent.setCentralWidget(m_origParentState.centralWidget);
}
