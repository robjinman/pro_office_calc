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
  auto& parentData = parentFragData<FSettingsDialogData>();

  m_data.eventSystem = parentData.eventSystem;
  m_data.updateLoop = parentData.updateLoop;

  setMouseTracking(true);

  m_data.pages.reset(new QStackedLayout(this));
  setLayout(m_data.pages.get());

  m_data.consoleLaunchPage.widget.reset(new QWidget);
  m_data.consoleLaunchPage.wgtToConsole.reset(new QPushButton("Admin console"));
  m_data.consoleLaunchPage.vbox.reset(new QVBoxLayout);
  m_data.consoleLaunchPage.vbox->addWidget(m_data.consoleLaunchPage.wgtToConsole.get());
  m_data.consoleLaunchPage.widget->setLayout(m_data.consoleLaunchPage.vbox.get());

  connect(m_data.consoleLaunchPage.wgtToConsole.get(), SIGNAL(clicked()), this,
    SLOT(onEnterConsoleClick()));

  m_data.pages->addWidget(m_data.consoleLaunchPage.widget.get());

  m_data.consoleAreYouSurePage.widget.reset(new QWidget);
  m_data.consoleAreYouSurePage.wgtAreYouSure.reset(new AreYouSureWidget(*m_data.eventSystem));
  m_data.consoleAreYouSurePage.vbox.reset(new QVBoxLayout);
  m_data.consoleAreYouSurePage.vbox->addWidget(m_data.consoleAreYouSurePage.wgtAreYouSure.get());
  m_data.consoleAreYouSurePage.widget->setLayout(m_data.consoleAreYouSurePage.vbox.get());

  m_data.pages->addWidget(m_data.consoleAreYouSurePage.widget.get());

  m_data.consolePage.widget.reset(new QWidget);
  m_data.consolePage.wgtConsole.reset(new ConsoleWidget);
  m_data.consolePage.wgtBack.reset(new QPushButton("Back"));
  m_data.consolePage.vbox.reset(new QVBoxLayout);
  m_data.consolePage.vbox->addWidget(m_data.consolePage.wgtConsole.get());
  m_data.consolePage.vbox->addWidget(m_data.consolePage.wgtBack.get());
  m_data.consolePage.widget->setLayout(m_data.consolePage.vbox.get());

  connect(m_data.consolePage.wgtBack.get(), SIGNAL(clicked()), this, SLOT(onExitConsoleClick()));

  m_data.pages->addWidget(m_data.consolePage.widget.get());

  m_areYouSureFailId = m_data.eventSystem->listen("areYouSureFail", [=](const Event&) {
    this->onAreYouSureFail();
  });
  m_areYouSurePassId = m_data.eventSystem->listen("areYouSurePass", [=](const Event&) {
    this->onAreYouSurePass();
  });

  parentData.vbox->addWidget(this);

  Fragment::rebuild(spec_);
}

//===========================================
// FConfigMaze::onAreYouSureFail
//===========================================
void FConfigMaze::onAreYouSureFail() {
  m_data.pages->setCurrentIndex(0);
}

//===========================================
// FConfigMaze::onAreYouSurePass
//===========================================
void FConfigMaze::onAreYouSurePass() {
  m_data.pages->setCurrentIndex(2);
}

//===========================================
// FConfigMaze::onEnterConsoleClick
//===========================================
void FConfigMaze::onEnterConsoleClick() {
  m_data.pages->setCurrentIndex(1);
  m_data.consoleAreYouSurePage.wgtAreYouSure->restart();
}

//===========================================
// FConfigMaze::onExitConsoleClick
//===========================================
void FConfigMaze::onExitConsoleClick() {
  m_data.pages->setCurrentIndex(0);
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
