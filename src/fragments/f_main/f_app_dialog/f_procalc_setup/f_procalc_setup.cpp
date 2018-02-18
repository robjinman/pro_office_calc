#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/f_procalc_setup.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/f_procalc_setup_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FProcalcSetup::FProcalcSetup
//===========================================
FProcalcSetup::FProcalcSetup(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FProcalcSetup", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FProcalcSetup::FProcalcSetup\n");
}

//===========================================
// FProcalcSetup::reload
//===========================================
void FProcalcSetup::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FProcalcSetup::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_data.wgtList = makeQtObjPtr<QListWidget>();
  m_data.wgtNext = makeQtObjPtr<QPushButton>("Next");

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  m_data.vbox->addWidget(m_data.wgtList.get());
  m_data.vbox->addWidget(m_data.wgtNext.get());

  setLayout(m_data.vbox.get());
}

//===========================================
// FProcalcSetup::cleanUp
//===========================================
void FProcalcSetup::cleanUp() {
  DBG_PRINT("FProcalcSetup::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FProcalcSetup::~FProcalcSetup
//===========================================
FProcalcSetup::~FProcalcSetup() {
  DBG_PRINT("FProcalcSetup::~FProcalcSetup\n");
}
