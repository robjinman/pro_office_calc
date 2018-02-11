#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen_spec.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FLoginScreen::FLoginScreen
//===========================================
FLoginScreen::FLoginScreen(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QLabel(nullptr),
    Fragment("FLoginScreen", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FLoginScreen::FLoginScreen\n");
}

//===========================================
// FLoginScreen::reload
//===========================================
void FLoginScreen::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FLoginScreen::reload\n");

  auto& parent = parentFrag<FMain>();

  commonData.eventSystem.listen("PasswordGeneratedEvent", [this](const Event& event_) {
    auto& event = dynamic_cast<const PasswordGeneratedEvent&>(event_);
    DBG_PRINT_VAR(event.password);
    m_data.password = event.password;
  }, m_pwdGenEventId);

  m_origParentState.centralWidget = parent.centralWidget();
  parent.setCentralWidget(this);

  m_data.wgtUser.reset(new QLineEdit(this));
  m_data.wgtUser->setGeometry(205, 170, 100, 20);

  m_data.wgtPassword.reset(new QLineEdit(this));
  m_data.wgtPassword->setGeometry(205, 195, 100, 20);

  connect(m_data.wgtPassword.get(), SIGNAL(returnPressed()), this, SLOT(onLoginAttempt()));
}

//===========================================
// FLoginScreen::onLoginAttempt
//===========================================
void FLoginScreen::onLoginAttempt() {
  if (m_data.wgtUser->text() == "rob" &&
    m_data.wgtPassword->text().toStdString() == m_data.password) {

    DBG_PRINT("Login success!\n");

    commonData.eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_ITS_RAINING_TETROMINOS)));
  }
  else {
    DBG_PRINT("Access denied\n");
    // TODO
  }
}

//===========================================
// FLoginScreen::cleanUp
//===========================================
void FLoginScreen::cleanUp() {
  DBG_PRINT("FLoginScreen::cleanUp\n");

  auto& parent = parentFrag<FMain>();

  parent.setCentralWidget(m_origParentState.centralWidget);
  commonData.eventSystem.forget(m_pwdGenEventId);
}

//===========================================
// FLoginScreen::~FLoginScreen
//===========================================
FLoginScreen::~FLoginScreen() {
  DBG_PRINT("FLoginScreen::~FLoginScreen\n");
}
