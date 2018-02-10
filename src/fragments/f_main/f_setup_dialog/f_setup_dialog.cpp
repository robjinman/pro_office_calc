#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_setup_dialog/f_setup_dialog.hpp"
#include "fragments/f_main/f_setup_dialog/f_setup_dialog_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FSetupDialog::FSetupDialog
//===========================================
FSetupDialog::FSetupDialog(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FSetupDialog", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FSetupDialog::FSetupDialog\n");

  commonData.eventSystem.listen("procalcLaunch", [this](const Event&) {
    exec();
  }, m_eventIdx);
}

//===========================================
// FSetupDialog::reload
//===========================================
void FSetupDialog::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FSetupDialog::reload\n");

  auto& spec = dynamic_cast<const FSetupDialogSpec&>(spec_);

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);
}

//===========================================
// FSetupDialog::cleanUp
//===========================================
void FSetupDialog::cleanUp() {
  DBG_PRINT("FSetupDialog::cleanUp\n");

  commonData.eventSystem.forget(m_eventIdx);
}

//===========================================
// FSetupDialog::~FSetupDialog
//===========================================
FSetupDialog::~FSetupDialog() {
  DBG_PRINT("FSetupDialog::~FSetupDialog\n");
}
