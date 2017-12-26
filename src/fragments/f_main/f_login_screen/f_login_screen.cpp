#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FLoginScreen::FLoginScreen
//===========================================
FLoginScreen::FLoginScreen(Fragment& parent_, FragmentData& parentData_)
  : QLabel(nullptr),
    Fragment("FLoginScreen", parent_, parentData_, m_data) {

  auto& parentData = dynamic_cast<FMainData&>(parentData_);
  m_data.eventSystem = &parentData.eventSystem;
}

//===========================================
// FLoginScreen::rebuild
//===========================================
void FLoginScreen::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FMain>();

  m_data.eventSystem->listen("PasswordGeneratedEvent", [this](const Event& event_) {
    auto& event = dynamic_cast<const PasswordGeneratedEvent&>(event_);
    m_data.password = event.password;
  });

  m_origParentState.centralWidget = parent.centralWidget();
  parent.setCentralWidget(this);

  auto& spec = dynamic_cast<const FLoginScreenSpec&>(spec_);

  QPixmap tmp(spec.backgroundImage);
  m_data.background.reset(new QPixmap(tmp.scaledToHeight(parent.size().height())));

  m_data.wgtUser.reset(new QLineEdit(this));
  m_data.wgtUser->setGeometry(205, 170, 100, 20);

  m_data.wgtPassword.reset(new QLineEdit(this));
  m_data.wgtPassword->setGeometry(205, 195, 100, 20);

  connect(m_data.wgtPassword.get(), SIGNAL(returnPressed()), this, SLOT(onLoginAttempt()));

  setPixmap(*m_data.background);

  Fragment::rebuild(spec_);
}

//===========================================
// FLoginScreen::onLoginAttempt
//===========================================
void FLoginScreen::onLoginAttempt() {
  if (m_data.wgtUser->text() == "rob" &&
    m_data.wgtPassword->text().toStdString() == m_data.password) {

    DBG_PRINT("Login success!\n");
  }
  else {
    DBG_PRINT("Access denied\n");
  }
}

//===========================================
// FLoginScreen::cleanUp
//===========================================
void FLoginScreen::cleanUp() {
  auto& parent = parentFrag<FMain>();

  parent.setCentralWidget(m_origParentState.centralWidget);
  m_data.eventSystem->forget(m_pwdGenEventId);
}

//===========================================
// FLoginScreen::~FLoginScreen
//===========================================
FLoginScreen::~FLoginScreen() {
  m_data.eventSystem->forget(m_pwdGenEventId);
}
