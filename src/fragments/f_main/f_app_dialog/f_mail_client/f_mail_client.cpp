#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_mail_client/f_mail_client.hpp"
#include "fragments/f_main/f_app_dialog/f_mail_client/f_mail_client_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FMailClient::FMailClient
//===========================================
FMailClient::FMailClient(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FMailClient", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FMailClient::FMailClient\n");
}

//===========================================
// FMailClient::reload
//===========================================
void FMailClient::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FMailClient::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_data.wgtLabel.reset(new QLabel("Mail Client"));

  m_data.vbox.reset(new QVBoxLayout);
  m_data.vbox->addWidget(m_data.wgtLabel.get());

  setLayout(m_data.vbox.get());
}

//===========================================
// FMailClient::cleanUp
//===========================================
void FMailClient::cleanUp() {
  DBG_PRINT("FMailClient::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FMailClient::~FMailClient
//===========================================
FMailClient::~FMailClient() {
  DBG_PRINT("FMailClient::~FMailClient\n");
}
