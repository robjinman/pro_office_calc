#include <QMessageBox>
#include <QMenuBar>
#include "fragments/f_main/f_about_dialog/f_about_dialog.hpp"
#include "fragments/f_main/f_about_dialog/f_about_dialog_spec.hpp"
#include "fragments/f_main/f_main.hpp"
#include "utils.hpp"


//===========================================
// FAboutDialog::FAboutDialog
//===========================================
FAboutDialog::FAboutDialog(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FAboutDialog", parent_, parentData_, m_data) {

  DBG_PRINT("FAboutDialog::FAboutDialog\n");
}

//===========================================
// FAboutDialog::initialise
//===========================================
void FAboutDialog::initialise(const FragmentSpec& spec_) {
  DBG_PRINT("FAboutDialog::initialise\n");
}

//===========================================
// FAboutDialog::reload
//===========================================
void FAboutDialog::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FAboutDialog::reload\n");

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
}

//===========================================
// FAboutDialog::cleanUp
//===========================================
void FAboutDialog::cleanUp() {
  DBG_PRINT("FAboutDialog::cleanUp\n");

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

//===========================================
// FAboutDialog::~FAboutDialog
//===========================================
FAboutDialog::~FAboutDialog() {
  DBG_PRINT("FAboutDialog::~FAboutDialog\n");
}
