#include <QMouseEvent>
#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze_spec.hpp"
#include "utils.hpp"
#include "event_system.hpp"


//===========================================
// FConfigMaze::FConfigMaze
//===========================================
FConfigMaze::FConfigMaze(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FConfigMaze", parent_, parentData_, m_data) {}

//===========================================
// FConfigMaze::rebuild
//===========================================
void FConfigMaze::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  m_data.eventSystem = parentData.eventSystem;
  m_data.updateLoop = parentData.updateLoop;

  setMouseTracking(true);

  m_data.wgtButton.reset(new QPushButton("Admin console"));
  m_data.wgtAreYouSure.reset(new AreYouSureWidget(*m_data.eventSystem));

  m_data.grid.reset(new QGridLayout);
  m_data.grid->addWidget(m_data.wgtButton.get(), 0, 0);

  connect(m_data.wgtButton.get(), SIGNAL(clicked()), this, SLOT(onBtnClick()));

  m_areYouSureFailId = m_data.eventSystem->listen("areYouSureFail", [=](const Event&) {
    this->onAreYouSureFail();
  });
  m_areYouSurePassId = m_data.eventSystem->listen("areYouSurePass", [=](const Event&) {
    this->onAreYouSurePass();
  });

  auto& spec = dynamic_cast<const FConfigMazeSpec&>(spec_);

  setLayout(m_data.grid.get());

  parentData.vbox->addWidget(this);

  Fragment::rebuild(spec_);
}

//===========================================
// FConfigMaze::onAreYouSureFail
//===========================================
void FConfigMaze::onAreYouSureFail() {
  DBG_PRINT("FConfigMaze::onAreYouSureFail");
  showContinueToAdminConsole();
}

//===========================================
// FConfigMaze::onAreYouSurePass
//===========================================
void FConfigMaze::onAreYouSurePass() {
  DBG_PRINT("FConfigMaze::onAreYouSurePass");
  // TODO
}

//===========================================
// FConfigMaze::showContinueToAdminConsole
//===========================================
void FConfigMaze::showContinueToAdminConsole() {
  DBG_PRINT("FConfigMaze::showContinueToAdminConsole\n");

  m_data.grid->removeWidget(m_data.wgtAreYouSure.get());
  m_data.wgtAreYouSure->setParent(nullptr);
  m_data.grid->addWidget(m_data.wgtButton.get(), 0, 0);
}

//===========================================
// FConfigMaze::showAreYouSure
//===========================================
void FConfigMaze::showAreYouSure() {
  DBG_PRINT("FConfigMaze::showAreYouSure\n");

  m_data.grid->removeWidget(m_data.wgtButton.get());
  m_data.wgtButton->setParent(nullptr);
  m_data.grid->addWidget(m_data.wgtAreYouSure.get(), 0, 0);
}

//===========================================
// FConfigMaze::onBtnClick
//===========================================
void FConfigMaze::onBtnClick() {
  showAreYouSure();
}

//===========================================
// FConfigMaze::cleanUp
//===========================================
void FConfigMaze::cleanUp() {
  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->removeWidget(this);
}

//===========================================
// FConfigMaze::~FConfigMaze
//===========================================
FConfigMaze::~FConfigMaze() {
  m_data.eventSystem->forget(m_areYouSureFailId);
  m_data.eventSystem->forget(m_areYouSurePassId);
}
