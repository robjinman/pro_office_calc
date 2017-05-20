#include <QMessageBox>
#include "fragments/f_main/f_about_dialog/f_about_dialog.hpp"
#include "fragments/f_main/f_about_dialog/f_about_dialog_spec.hpp"
#include "fragments/f_main/f_main.hpp"


//===========================================
// FAboutDialog::FAboutDialog
//===========================================
FAboutDialog::FAboutDialog(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FAboutDialog", parent_, parentData_, m_data) {

  auto& parentData = parentFragData<FMainData>();
  auto& parent = parentFrag<FMain>();

  m_actAbout.reset(new QAction("About", &parent));
  parentData.mnuHelp->addAction(m_actAbout.get());

  connect(m_actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));
}

//===========================================
// FAboutDialog::rebuild
//===========================================
void FAboutDialog::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FAboutDialogSpec&>(spec_);
  m_text = spec.text;

  Fragment::rebuild(spec_);
}

//===========================================
// FAboutDialog::cleanUp
//===========================================
void FAboutDialog::cleanUp() {

}

//===========================================
// FAboutDialog::showAbout
//===========================================
void FAboutDialog::showAbout() {
  QMessageBox msgBox(&parentFrag<FMain>());
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setWindowTitle("About");
  msgBox.setText(m_text);
  msgBox.exec();
}
