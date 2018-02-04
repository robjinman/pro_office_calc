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

  m_data.wgtTabs.reset(new QTabWidget(this));
  m_data.vbox->addWidget(m_data.wgtTabs.get());

  setupTab1();
  setupTab2();
  setupTab3();

  m_data.wgtTabs->addTab(m_data.tab1.page.get(), "Auto");
  m_data.wgtTabs->addTab(m_data.tab2.page.get(), "Manual");
  m_data.wgtTabs->addTab(m_data.tab3.page.get(), "Blah");

  connect(m_data.actPreferences.get(), SIGNAL(triggered()), this, SLOT(showTroubleshooterDialog()));

  auto& spec = dynamic_cast<const FTroubleshooterDialogSpec&>(spec_);

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);
}

//===========================================
// FTroubleshooterDialog::setupTab1
//===========================================
void FTroubleshooterDialog::setupTab1() {
  auto& tab = m_data.tab1;

  tab.page.reset(new QWidget);
  tab.vbox.reset(new QVBoxLayout);
  tab.page->setLayout(tab.vbox.get());
  tab.wgtCaption.reset(new QLabel("Attempt to fix problem automatically"));
  tab.wgtRunTroubleshooter.reset(new QPushButton("Run troubleshooter"));
  tab.wgtProgressBar.reset(new QProgressBar);
  tab.wgtGroupbox.reset(new QGroupBox("Result"));
  tab.resultsVbox.reset(new QVBoxLayout);
  tab.btnsHbox.reset(new QHBoxLayout);
  tab.wgtNoProblemsFound.reset(new QLabel("> No problems found"));
  tab.wgtProblemResolved.reset(new QLabel("Has this resolved the problem?"));
  tab.wgtYes.reset(new QPushButton("Yes"));
  tab.wgtNo.reset(new QPushButton("No"));

  tab.btnsHbox->addWidget(tab.wgtYes.get());
  tab.btnsHbox->addWidget(tab.wgtNo.get());

  tab.resultsVbox->addWidget(tab.wgtNoProblemsFound.get());
  tab.resultsVbox->addWidget(tab.wgtProblemResolved.get());
  tab.resultsVbox->addLayout(tab.btnsHbox.get());

  tab.wgtGroupbox->setLayout(tab.resultsVbox.get());

  tab.vbox->addWidget(tab.wgtCaption.get());
  tab.vbox->addWidget(tab.wgtRunTroubleshooter.get());
  tab.vbox->addWidget(tab.wgtProgressBar.get());
  tab.vbox->addWidget(tab.wgtGroupbox.get(), 1);
}

//===========================================
// FTroubleshooterDialog::setupTab2
//===========================================
void FTroubleshooterDialog::setupTab2() {

}

//===========================================
// FTroubleshooterDialog::setupTab3
//===========================================
void FTroubleshooterDialog::setupTab3() {

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
