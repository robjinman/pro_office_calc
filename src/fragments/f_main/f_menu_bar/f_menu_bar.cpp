#include <QMessageBox>
#include <QMenuBar>
#include "fragments/f_main/f_menu_bar/f_menu_bar.hpp"
#include "fragments/f_main/f_menu_bar/f_menu_bar_spec.hpp"
#include "fragments/f_main/f_main.hpp"


//===========================================
// FMenuBar::FMenuBar
//===========================================
FMenuBar::FMenuBar(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FMenuBar", parent_, parentData_, m_data) {

  auto& parent = parentFrag<FMain>();

  m_data.mnuFile.reset(parent.menuBar()->addMenu("File"));
  m_data.actQuit.reset(new QAction("Quit", this));
  m_data.mnuFile->addAction(m_data.actQuit.get());

  m_data.mnuHelp.reset(parent.menuBar()->addMenu("Help"));
  m_data.actAbout.reset(new QAction("About", this));
  m_data.mnuHelp->addAction(m_data.actAbout.get());

  connect(m_data.actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));
  connect(m_data.actQuit.get(), SIGNAL(triggered()), this, SLOT(onClose()));
}

//===========================================
// FMenuBar::close
//===========================================
void FMenuBar::onClose() {
  parentFrag<FMain>().close();
}

//===========================================
// FMenuBar::rebuild
//===========================================
void FMenuBar::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FMenuBarSpec&>(spec_);

  m_aboutDialogTitle = spec.aboutDialogTitle;
  m_aboutDialogText = spec.aboutDialogText;

  m_data.mnuFile->setTitle(spec.fileLabel);
  m_data.actQuit->setText(spec.quitLabel);
  m_data.mnuHelp->setTitle(spec.helpLabel);
  m_data.actAbout->setText(spec.aboutLabel);

  Fragment::rebuild(spec_);
}

//===========================================
// FMenuBar::cleanUp
//===========================================
void FMenuBar::cleanUp() {
  parentFrag<FMain>().menuBar()->clear();
}

//===========================================
// FMenuBar::showAbout
//===========================================
void FMenuBar::showAbout() {
  QMessageBox msgBox(&parentFrag<FMain>());
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setWindowTitle(m_aboutDialogTitle);
  msgBox.setText(m_aboutDialogText);
  msgBox.exec();
}
