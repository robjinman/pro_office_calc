#include <QMenu>
#include <QMessageBox>
#include "fragments/f_main/f_normal_calc_about_dialog/f_normal_calc_about_dialog.hpp"
#include "fragments/f_main/f_normal_calc_about_dialog/f_normal_calc_about_dialog_spec.hpp"
#include "fragments/f_main/f_main.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FNormalCalcAboutDialog::FNormalCalcAboutDialog
//===========================================
FNormalCalcAboutDialog::FNormalCalcAboutDialog(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FNormalCalcAboutDialog", parent_, parentData_, m_data) {

  auto& parentData = parentFragData<FMainData>();
  auto& parent = parentFrag<FMain>();

  parentData.fnOnQuit = [this]() { onQuit(); };

  m_actAbout.reset(new QAction("About", &parent));
  parentData.mnuHelp->addAction(m_actAbout.get());

  connect(m_actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));
}

//===========================================
// FNormalCalcAboutDialog::rebuild
//===========================================
void FNormalCalcAboutDialog::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FNormalCalcAboutDialogSpec&>(spec_);
  m_stateId = spec.stateId;

  Fragment::rebuild(spec_);
}

//===========================================
// FNormalCalcAboutDialog::cleanUp
//===========================================
void FNormalCalcAboutDialog::cleanUp() {
  parentFragData<FMainData>().fnOnQuit = []() {};
}

//===========================================
// FNormalCalcAboutDialog::showAbout
//===========================================
void FNormalCalcAboutDialog::showAbout() {
  QMessageBox msgBox(&parentFrag<FMain>());
  msgBox.setTextFormat(Qt::RichText);

  int count = 10 - m_stateId;

  QString clue1;
  clue1 += "<i>" + QString::number(count) + "</i>";

  QString clue2 = "<font size=6>⚠∞</font>";

  QString msg;
  msg += "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
    "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
    "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
    + (count > 0 ? clue1 : clue2);

  msgBox.setWindowTitle("About");
  msgBox.setText(msg);
  msgBox.exec();
}

//===========================================
// FNormalCalcAboutDialog::onQuit
//===========================================
void FNormalCalcAboutDialog::onQuit() {
  DBG_PRINT("FNormalCalcAboutDialog::onQuit\n");

  if (m_stateId < ST_NORMAL_CALCULATOR_10) {
    parentFragData<FMainData>().eventSystem.fire(RequestStateChangeEvent(m_stateId + 1));
  }
}
