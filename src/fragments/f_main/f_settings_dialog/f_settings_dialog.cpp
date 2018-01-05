#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_settings_dialog_spec.hpp"
#include "utils.hpp"


//===========================================
// FSettingsDialog::FSettingsDialog
//===========================================
FSettingsDialog::FSettingsDialog(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FSettingsDialog", parent_, parentData_, m_data) {

  DBG_PRINT("FSettingsDialog::FSettingsDialog\n");

  auto& parentData = parentFragData<FMainData>();

  m_data.eventSystem = &parentData.eventSystem;
  m_data.updateLoop = &parentData.updateLoop;

  m_data.vbox.reset(new QVBoxLayout);
  setLayout(m_data.vbox.get());

  m_data.actSettings.reset(new QAction("Settings", this));
  parentData.mnuFile->addAction(m_data.actSettings.get());

  connect(m_data.actSettings.get(), SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
}

//===========================================
// FSettingsDialog::reload
//===========================================
void FSettingsDialog::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FSettingsDialog::reload\n");

  auto& spec = dynamic_cast<const FSettingsDialogSpec&>(spec_);

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);

  if (spec.backgroundImage.length() > 0) {
    setStyleSheet(QString("background-image:url(\"") + spec.backgroundImage + "\");");
  }
  else {
    setStyleSheet("");
  }
}

//===========================================
// FSettingsDialog::cleanUp
//===========================================
void FSettingsDialog::cleanUp() {
  DBG_PRINT("FSettingsDialog::cleanUp\n");

  auto& parentData = parentFragData<FMainData>();

  parentData.mnuFile->removeAction(m_data.actSettings.get());
}

//===========================================
// FSettingsDialog::showSettingsDialog
//===========================================
void FSettingsDialog::showSettingsDialog() {
  exec();
}

//===========================================
// FSettingsDialog::~FSettingsDialog
//===========================================
FSettingsDialog::~FSettingsDialog() {
  DBG_PRINT("FSettingsDialog::~FSettingsDialog\n");
}
