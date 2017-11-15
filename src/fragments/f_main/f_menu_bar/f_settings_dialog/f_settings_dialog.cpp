#include <QMessageBox>
#include <QMenuBar>
#include "fragments/f_main/f_menu_bar/f_menu_bar.hpp"
#include "fragments/f_main/f_menu_bar/f_menu_bar_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog_spec.hpp"


//===========================================
// FSettingsDialog::FSettingsDialog
//===========================================
FSettingsDialog::FSettingsDialog(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FSettingsDialog", parent_, parentData_, m_data) {}

//===========================================
// FSettingsDialog::rebuild
//===========================================
void FSettingsDialog::rebuild(const FragmentSpec& spec_) {
  auto& parentData = parentFragData<FMenuBarData>();

  m_data.eventSystem = parentData.eventSystem;

  m_data.vbox.reset(new QVBoxLayout);
  setLayout(m_data.vbox.get());

  m_data.actSettings.reset(new QAction("Settings", this));
  parentData.mnuFile->addAction(m_data.actSettings.get());

  connect(m_data.actSettings.get(), SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

  auto& spec = dynamic_cast<const FSettingsDialogSpec&>(spec_);

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);

  Fragment::rebuild(spec_);
}

//===========================================
// FSettingsDialog::cleanUp
//===========================================
void FSettingsDialog::cleanUp() {
  auto& parentData = parentFragData<FMenuBarData>();

  parentData.mnuFile->removeAction(m_data.actSettings.get());
}

//===========================================
// FSettingsDialog::showSettingsDialog
//===========================================
void FSettingsDialog::showSettingsDialog() {
  exec();
}
