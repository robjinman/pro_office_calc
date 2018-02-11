#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_mail_client/f_mail_client.hpp"
#include "fragments/f_main/f_mail_client/f_mail_client_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FMailClient::FMailClient
//===========================================
FMailClient::FMailClient(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FMailClient", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FMailClient::FMailClient\n");

  commonData.eventSystem.listen("mailClientLaunch", [this](const Event&) {
    show();
  }, m_eventIdx);
}

//===========================================
// FMailClient::reload
//===========================================
void FMailClient::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FMailClient::reload\n");

  auto& spec = dynamic_cast<const FMailClientSpec&>(spec_);

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);
}

//===========================================
// FMailClient::cleanUp
//===========================================
void FMailClient::cleanUp() {
  DBG_PRINT("FMailClient::cleanUp\n");

  commonData.eventSystem.forget(m_eventIdx);
}

//===========================================
// FMailClient::~FMailClient
//===========================================
FMailClient::~FMailClient() {
  DBG_PRINT("FMailClient::~FMailClient\n");
}
