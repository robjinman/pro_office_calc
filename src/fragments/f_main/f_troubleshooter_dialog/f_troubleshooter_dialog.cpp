#include <QMessageBox>
#include <QMenuBar>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_troubleshooter_dialog/f_troubleshooter_dialog.hpp"
#include "fragments/f_main/f_troubleshooter_dialog/f_troubleshooter_dialog_spec.hpp"
#include "utils.hpp"


//===========================================
// FTroubleshooterDialog::FTroubleshooterDialog
//===========================================
FTroubleshooterDialog::FTroubleshooterDialog(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FTroubleshooterDialog", parent_, parentData_, m_data) {

  DBG_PRINT("FTroubleshooterDialog::FTroubleshooterDialog\n");
}

//===========================================
// FTroubleshooterDialog::reload
//===========================================
void FTroubleshooterDialog::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FTroubleshooterDialog::reload\n");

  auto& parentData = parentFragData<FMainData>();

  m_data.eventSystem = &parentData.eventSystem;
  m_data.updateLoop = &parentData.updateLoop;

  m_data.vbox.reset(new QVBoxLayout);
  setLayout(m_data.vbox.get());

  m_data.actPreferences.reset(new QAction("Troubleshooter", this));
  parentData.mnuHelp->addAction(m_data.actPreferences.get());

  connect(m_data.actPreferences.get(), SIGNAL(triggered()), this, SLOT(showTroubleshooterDialog()));

  auto& spec = dynamic_cast<const FTroubleshooterDialogSpec&>(spec_);

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);
}

//===========================================
// FTroubleshooterDialog::cleanUp
//===========================================
void FTroubleshooterDialog::cleanUp() {
  DBG_PRINT("FTroubleshooterDialog::cleanUp\n");

  auto& parentData = parentFragData<FMainData>();

  parentData.mnuHelp->removeAction(m_data.actPreferences.get());
}

//===========================================
// FTroubleshooterDialog::showTroubleshooterDialog
//===========================================
void FTroubleshooterDialog::showTroubleshooterDialog() {
  exec();
}

//===========================================
// FTroubleshooterDialog::~FTroubleshooterDialog
//===========================================
FTroubleshooterDialog::~FTroubleshooterDialog() {
  DBG_PRINT("FTroubleshooterDialog::~FTroubleshooterDialog\n");
}
