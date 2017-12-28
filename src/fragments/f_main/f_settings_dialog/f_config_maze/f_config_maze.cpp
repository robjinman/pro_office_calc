#include <random>
#include <QMouseEvent>
#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze_spec.hpp"
#include "utils.hpp"
#include "event_system.hpp"


using std::string;


static std::random_device rd;


//===========================================
// generatePassword
//===========================================
static string generatePassword() {
  string syms("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
  std::uniform_int_distribution<int> randIdx(0, syms.length() - 1);
  std::uniform_int_distribution<int> randLen(8, 14);

  int len = randLen(rd);
  string pwd;

  for (int i = 0; i < len; ++i) {
    pwd.push_back(syms[randIdx(rd)]);
  }

  return pwd;
}

//===========================================
// FConfigMaze::FConfigMaze
//===========================================
FConfigMaze::FConfigMaze(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FConfigMaze", parent_, parentData_, m_data) {

  DBG_PRINT("FConfigMaze::FConfigMaze\n");
}

//===========================================
// FConfigMaze::rebuild
//===========================================
void FConfigMaze::rebuild(const FragmentSpec& spec_) {
  DBG_PRINT("FConfigMaze::rebuild\n");

  auto& parentData = parentFragData<FSettingsDialogData>();

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
  m_data.consoleAreYouSurePage.wgtAreYouSure.reset(new AreYouSureWidget);
  m_data.consoleAreYouSurePage.vbox.reset(new QVBoxLayout);
  m_data.consoleAreYouSurePage.vbox->addWidget(m_data.consoleAreYouSurePage.wgtAreYouSure.get());
  m_data.consoleAreYouSurePage.widget->setLayout(m_data.consoleAreYouSurePage.vbox.get());

  connect(m_data.consoleAreYouSurePage.wgtAreYouSure.get(), SIGNAL(finished(bool)), this,
    SLOT(onAreYouSureFinish(bool)));

  m_data.pages->addWidget(m_data.consoleAreYouSurePage.widget.get());

  string pwd = generatePassword();
  parentData.eventSystem->fire(pEvent_t(new PasswordGeneratedEvent(pwd)));

  m_data.consolePage.widget.reset(new QWidget);
  m_data.consolePage.wgtConsole.reset(new ConsoleWidget({
    "logouut",
    string("chpwd ") + pwd
  }));
  m_data.consolePage.wgtConsole->addCommand("logout", [](const ConsoleWidget::ArgList&) {
    return "An error occurred";
  });
  m_data.consolePage.wgtConsole->addCommand("chpwd", [](const ConsoleWidget::ArgList&) {
    return "An error occurred";
  });
  m_data.consolePage.wgtBack.reset(new QPushButton("Exit"));
  m_data.consolePage.wgtBack->setMaximumWidth(50);
  m_data.consolePage.vbox.reset(new QVBoxLayout);
  m_data.consolePage.vbox->addWidget(m_data.consolePage.wgtConsole.get());
  m_data.consolePage.vbox->addWidget(m_data.consolePage.wgtBack.get());
  m_data.consolePage.widget->setLayout(m_data.consolePage.vbox.get());

  connect(m_data.consolePage.wgtBack.get(), SIGNAL(clicked()), this, SLOT(onExitConsoleClick()));

  m_data.pages->addWidget(m_data.consolePage.widget.get());

  parentData.vbox->addWidget(this);

  Fragment::rebuild(spec_);
}

//===========================================
// FConfigMaze::onAreYouSureFinish
//===========================================
void FConfigMaze::onAreYouSureFinish(bool passed) {
  if (passed) {
    m_data.pages->setCurrentIndex(2);
  }
  else {
    m_data.pages->setCurrentIndex(0);
  }
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
  DBG_PRINT("FConfigMaze::cleanUp\n");

  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->removeWidget(this);
}

//===========================================
// FConfigMaze::~FConfigMaze
//===========================================
FConfigMaze::~FConfigMaze() {
  DBG_PRINT("FConfigMaze::~FConfigMaze\n");
}
