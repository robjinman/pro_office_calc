#include <cassert>
#include <QMessageBox>
#include <QMenuBar>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_troubleshooter_dialog/f_troubleshooter_dialog.hpp"
#include "fragments/f_main/f_troubleshooter_dialog/f_troubleshooter_dialog_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


using its_raining_tetrominos::GameLogic;


//===========================================
// FTroubleshooterDialog::FTroubleshooterDialog
//===========================================
FTroubleshooterDialog::FTroubleshooterDialog(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FTroubleshooterDialog", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FTroubleshooterDialog::FTroubleshooterDialog\n");
}

//===========================================
// FTroubleshooterDialog::reload
//===========================================
void FTroubleshooterDialog::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FTroubleshooterDialog::reload\n");

  auto& parentData = parentFragData<FMainData>();

  m_data.vbox.reset(new QVBoxLayout);
  setLayout(m_data.vbox.get());

  m_data.actPreferences.reset(new QAction("Troubleshooter", this));
  parentData.mnuHelp->addAction(m_data.actPreferences.get());

  m_data.wgtTabs.reset(new QTabWidget(this));
  m_data.vbox->addWidget(m_data.wgtTabs.get());

  setupTab1();
  setupTab2();
  setupTab3();

  m_data.wgtTabs->addTab(m_data.tab1.page.get(), "Fix automatically");

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

  tab.timer.reset(new QTimer);

  tab.wgtGroupbox->setLayout(tab.resultsVbox.get());
  tab.wgtGroupbox->setVisible(false);

  auto sp = tab.wgtGroupbox->sizePolicy();
  sp.setRetainSizeWhenHidden(true);
  tab.wgtGroupbox->setSizePolicy(sp);

  tab.wgtProgressBar->setVisible(false);
  tab.wgtProgressBar->setRange(0, 10);

  tab.vbox->addWidget(tab.wgtCaption.get());
  tab.vbox->addWidget(tab.wgtRunTroubleshooter.get());
  tab.vbox->addWidget(tab.wgtProgressBar.get());
  tab.vbox->addWidget(tab.wgtGroupbox.get(), 1);

  connect(tab.wgtRunTroubleshooter.get(), SIGNAL(clicked()), this, SLOT(onRunTroubleshooter()));
  connect(tab.timer.get(), SIGNAL(timeout()), this, SLOT(onTick()));
  connect(tab.wgtNo.get(), SIGNAL(clicked()), this, SLOT(onNoClick()));
  connect(tab.wgtYes.get(), SIGNAL(clicked()), this, SLOT(onYesClick()));
}

//===========================================
// FTroubleshooterDialog::setupTab2
//===========================================
void FTroubleshooterDialog::setupTab2() {
  auto& tab = m_data.tab2;

  tab.page.reset(new QWidget);
  tab.vbox.reset(new QVBoxLayout);
  tab.wgtTextBrowser.reset(new QTextBrowser);

  tab.vbox->addWidget(tab.wgtTextBrowser.get());

  tab.wgtTextBrowser->setSearchPaths(QStringList() << "data/its_raining_tetrominos");
  tab.wgtTextBrowser->setSource(QUrl("troubleshooter1.html"));

  tab.page->setLayout(tab.vbox.get());
}

//===========================================
// FTroubleshooterDialog::setupTab3
//===========================================
void FTroubleshooterDialog::setupTab3() {
  auto& tab = m_data.tab3;

  tab.page.reset(new QWidget);
  tab.vbox.reset(new QVBoxLayout);
  tab.wgtRaycast.reset(new RaycastWidget(this, commonData.eventSystem));
  tab.wgtRaycast->initialise("data/its_raining_tetrominos/maze.svg");
  tab.gameLogic.reset(new GameLogic(commonData.eventSystem, tab.wgtRaycast->entityManager()));

  tab.vbox->addWidget(tab.wgtRaycast.get());

  tab.page->setLayout(tab.vbox.get());
}

//===========================================
// FTroubleshooterDialog::onNoClick
//===========================================
void FTroubleshooterDialog::onNoClick() {
  assert(m_data.wgtTabs->count() == 2);

  m_data.tab1.wgtGroupbox->setVisible(false);

  m_data.wgtTabs->insertTab(1, m_data.tab2.page.get(), "Common problems");
  m_data.wgtTabs->setCurrentIndex(1);
  m_data.tab2.wgtTextBrowser->setSource(QUrl("troubleshooter1.html"));
}

//===========================================
// FTroubleshooterDialog::onYesClick
//===========================================
void FTroubleshooterDialog::onYesClick() {
  auto& tab = m_data.tab1;

  tab.wgtGroupbox->setVisible(false);
}

//===========================================
// FTroubleshooterDialog::onTick
//===========================================
void FTroubleshooterDialog::onTick() {
  auto& tab = m_data.tab1;

  if (tab.wgtProgressBar->value() == 10) {
    tab.timer->stop();

    tab.wgtProgressBar->setVisible(false);
    tab.wgtGroupbox->setVisible(true);
    tab.wgtRunTroubleshooter->setDisabled(false);
    m_data.wgtTabs->addTab(m_data.tab3.page.get(), "");

    commonData.eventSystem.fire(pEvent_t(new Event("increaseTetrominoRain")));
  }
  else {
    tab.wgtProgressBar->setValue(tab.wgtProgressBar->value() + 1);
  }
}

//===========================================
// FTroubleshooterDialog::onRunTroubleshooter
//===========================================
void FTroubleshooterDialog::onRunTroubleshooter() {
  auto& tab = m_data.tab1;

  m_data.wgtTabs->removeTab(2);
  m_data.wgtTabs->removeTab(1);

  tab.wgtProgressBar->setVisible(true);
  tab.wgtGroupbox->setVisible(false);
  tab.wgtRunTroubleshooter->setDisabled(true);

  tab.wgtProgressBar->setValue(0);
  tab.timer->start(300);
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
  show();
}

//===========================================
// FTroubleshooterDialog::~FTroubleshooterDialog
//===========================================
FTroubleshooterDialog::~FTroubleshooterDialog() {
  DBG_PRINT("FTroubleshooterDialog::~FTroubleshooterDialog\n");
}
