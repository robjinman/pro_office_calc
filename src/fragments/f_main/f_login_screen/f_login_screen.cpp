#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen_spec.hpp"
#include "state_ids.hpp"
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
void FLoginScreen::reload(const FragmentSpec&) {
  DBG_PRINT("FLoginScreen::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_hPwdGen = commonData.eventSystem.listen("passwordGeneratedEvent", [this](const Event& event_) {
    auto& event = dynamic_cast<const PasswordGeneratedEvent&>(event_);
    DBG_PRINT_VAR(event.password);
    m_data.password = event.password;
  });

  m_data.wgtUser = makeQtObjPtr<QLineEdit>(this);
  m_data.wgtUser->setGeometry(205, 160, 100, 20);

  m_data.wgtPassword = makeQtObjPtr<QLineEdit>(this);
  m_data.wgtPassword->setGeometry(205, 185, 100, 20);

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

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FLoginScreen::~FLoginScreen
//===========================================
FLoginScreen::~FLoginScreen() {
  DBG_PRINT("FLoginScreen::~FLoginScreen\n");
}
