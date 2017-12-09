#include <QMessageBox>
#include <QMenuBar>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_preferences_dialog/f_preferences_dialog.hpp"
#include "fragments/f_main/f_preferences_dialog/f_preferences_dialog_spec.hpp"


//===========================================
// FPreferencesDialog::FPreferencesDialog
//===========================================
FPreferencesDialog::FPreferencesDialog(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FPreferencesDialog", parent_, parentData_, m_data) {}

//===========================================
// FPreferencesDialog::rebuild
//===========================================
void FPreferencesDialog::rebuild(const FragmentSpec& spec_) {
  auto& parentData = parentFragData<FMainData>();

  m_data.eventSystem = &parentData.eventSystem;
  m_data.updateLoop = &parentData.updateLoop;

  m_data.vbox.reset(new QVBoxLayout);
  setLayout(m_data.vbox.get());

  m_data.actPreferences.reset(new QAction("Preferences", this));
  parentData.mnuFile->addAction(m_data.actPreferences.get());

  connect(m_data.actPreferences.get(), SIGNAL(triggered()), this, SLOT(showPreferencesDialog()));

  auto& spec = dynamic_cast<const FPreferencesDialogSpec&>(spec_);

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);

  Fragment::rebuild(spec_);
}

//===========================================
// FPreferencesDialog::cleanUp
//===========================================
void FPreferencesDialog::cleanUp() {
  auto& parentData = parentFragData<FMainData>();

  parentData.mnuFile->removeAction(m_data.actPreferences.get());
}

//===========================================
// FPreferencesDialog::showPreferencesDialog
//===========================================
void FPreferencesDialog::showPreferencesDialog() {
  exec();
}
