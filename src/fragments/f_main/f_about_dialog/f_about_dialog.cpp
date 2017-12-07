#include <QMessageBox>
#include <QMenuBar>
#include "fragments/f_main/f_about_dialog/f_about_dialog.hpp"
#include "fragments/f_main/f_about_dialog/f_about_dialog_spec.hpp"
#include "fragments/f_main/f_main.hpp"


//===========================================
// FAboutDialog::FAboutDialog
//===========================================
FAboutDialog::FAboutDialog(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FAboutDialog", parent_, parentData_, m_data) {}

//===========================================
// FAboutDialog::rebuild
//===========================================
void FAboutDialog::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FMain>();
  auto& parentData = parentFragData<FMainData>();

  m_data.mnuHelp.reset(parent.menuBar()->addMenu("Help"));
  m_data.actAbout.reset(new QAction("About", this));
  m_data.mnuHelp->addAction(m_data.actAbout.get());

  connect(m_data.actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));

  auto& spec = dynamic_cast<const FAboutDialogSpec&>(spec_);

  m_aboutDialogTitle = spec.aboutDialogTitle;
  m_aboutDialogText = spec.aboutDialogText;

  m_data.mnuHelp->setTitle(spec.helpLabel);
  m_data.actAbout->setText(spec.aboutLabel);

  Fragment::rebuild(spec_);
}

//===========================================
// FAboutDialog::cleanUp
//===========================================
void FAboutDialog::cleanUp() {
  // TODO: Not ideal, as the menu bar will contain items from other fragments
  parentFrag<FMain>().menuBar()->clear();
}

//===========================================
// FAboutDialog::showAbout
//===========================================
void FAboutDialog::showAbout() {
  QMessageBox msgBox(&parentFrag<FMain>());
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setWindowTitle(m_aboutDialogTitle);
  msgBox.setText(m_aboutDialogText);
  msgBox.exec();
}
