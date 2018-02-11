#include <random>
#include <algorithm>
#include <QMouseEvent>
#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze_spec.hpp"
#include "utils.hpp"
#include "event_system.hpp"
#include "strings.hpp"


using std::string;


static std::random_device rd;
static std::mt19937 randEngine(rd());


//===========================================
// generatePassword
//===========================================
static string generatePassword() {
  string syms("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
  std::uniform_int_distribution<int> randIdx(0, syms.length() - 1);
  std::uniform_int_distribution<int> randLen(8, 14);

  int len = randLen(randEngine);
  string pwd;

  for (int i = 0; i < len; ++i) {
    pwd.push_back(syms[randIdx(randEngine)]);
  }

  return pwd;
}

//===========================================
// FConfigMaze::FConfigMaze
//===========================================
FConfigMaze::FConfigMaze(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FConfigMaze", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FConfigMaze::FConfigMaze\n");

  auto& parentData = parentFragData<FSettingsDialogData>();

  setMouseTracking(true);

  m_data.stackedLayout.reset(new QStackedLayout(this));
  setLayout(m_data.stackedLayout.get());

  constructConsoleLaunchPage();
  constructAreYouSurePage();
  constructConsolePage();

  m_data.stackedLayout->addWidget(m_data.consoleLaunchPage.widget.get());
  m_data.stackedLayout->addWidget(m_data.consoleAreYouSurePage.widget.get());
  m_data.stackedLayout->addWidget(m_data.consolePage.widget.get());

  m_layoutIdxOfConsoleLaunchPage = 0;
  m_layoutIdxOfAreYouSurePage = 1;
  m_layoutIdxOfConsolePage = 2;
  m_layoutIdxOfFirstConfigPage = 3;

  parentData.vbox->addWidget(this);
}

//===========================================
// FConfigMaze::constructConsoleLaunchPage
//===========================================
void FConfigMaze::constructConsoleLaunchPage() {
  m_data.consoleLaunchPage.widget.reset(new QWidget);
  m_data.consoleLaunchPage.wgtToConsole.reset(new QPushButton("Admin console"));
  m_data.consoleLaunchPage.vbox.reset(new QVBoxLayout);
  m_data.consoleLaunchPage.vbox->addWidget(m_data.consoleLaunchPage.wgtToConsole.get());
  m_data.consoleLaunchPage.widget->setLayout(m_data.consoleLaunchPage.vbox.get());

  connect(m_data.consoleLaunchPage.wgtToConsole.get(), SIGNAL(clicked()), this,
    SLOT(onEnterConsoleClick()));
}

//===========================================
// FConfigMaze::constructAreYouSurePage
//===========================================
void FConfigMaze::constructAreYouSurePage() {
  m_data.consoleAreYouSurePage.widget.reset(new QWidget);
  m_data.consoleAreYouSurePage.wgtAreYouSure.reset(new AreYouSureWidget);
  m_data.consoleAreYouSurePage.vbox.reset(new QVBoxLayout);
  m_data.consoleAreYouSurePage.vbox->addWidget(m_data.consoleAreYouSurePage.wgtAreYouSure.get());
  m_data.consoleAreYouSurePage.widget->setLayout(m_data.consoleAreYouSurePage.vbox.get());

  connect(m_data.consoleAreYouSurePage.wgtAreYouSure.get(), SIGNAL(finished(bool)), this,
    SLOT(onAreYouSureFinish(bool)));
}

//===========================================
// FConfigMaze::constructConsolePage
//===========================================
void FConfigMaze::constructConsolePage() {
  string pwd = generatePassword();
  commonData.eventSystem.fire(pEvent_t(new PasswordGeneratedEvent(pwd)));

  string initialContent =
    "┌───────────────────────────────────────┐\n"
    "│ Admin Console v1.1.16                 │\n"
    "├───────────────────────────────────────┤\n"
    "│ Date and time    1993/03/14 16:11:23  │\n"
    "│ Logged in as     rob                  │\n"
    "│ Logged in since  1993/03/14 15:22:49  │\n"
    "│                                       │\n"
    "│ ↑↓ cycle history                      │\n"
    "└───────────────────────────────────────┘\n"
    "> ";

  m_data.consolePage.widget.reset(new QWidget);
  m_data.consolePage.wgtConsole.reset(new ConsoleWidget(initialContent, {
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
}

//===========================================
// FConfigMaze::reload
//===========================================
void FConfigMaze::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FConfigMaze::reload\n");

  auto& spec = dynamic_cast<const FConfigMazeSpec&>(spec_);

  ucs4string_t symbols_ = utf8ToUcs4(spec.symbols);
  std::shuffle(symbols_.begin(), symbols_.end(), randEngine);
  QString symbols(ucs4ToUtf8(symbols_).c_str());

  m_data.pages[0].reset(new ConfigPage(symbols[0], { 1, 2 }));
  m_data.pages[1].reset(new ConfigPage(symbols[1], { 0, 3 }));
  m_data.pages[2].reset(new ConfigPage(symbols[2], { 0, 3 }));
  m_data.pages[3].reset(new ConfigPage(symbols[3], { 1, 2, 4, 5 }));
  m_data.pages[4].reset(new ConfigPage(symbols[4], { 3, 6 }));
  m_data.pages[5].reset(new ConfigPage(symbols[5], { 3, 6, 9 }));
  m_data.pages[6].reset(new ConfigPage(symbols[6], { 4, 5, 7, 10 }));
  m_data.pages[7].reset(new ConfigPage(symbols[7], { 6, 11 }));
  m_data.pages[8].reset(new ConfigPage(symbols[8], { 9, 13 }));
  m_data.pages[9].reset(new ConfigPage(symbols[9], { 5, 8, 10, 14 }));
  m_data.pages[10].reset(new ConfigPage(symbols[10], { 6, 9, 11 }));
  m_data.pages[11].reset(new ConfigPage(symbols[11], { 7, 10, 12, 15 }));
  m_data.pages[12].reset(new ConfigPage(symbols[12], { 11, 100 }));
  m_data.pages[13].reset(new ConfigPage(symbols[13], { 8, 14 }));
  m_data.pages[14].reset(new ConfigPage(symbols[14], { 9, 13 }));
  m_data.pages[15].reset(new ConfigPage(symbols[15], { 11, 100 }));

  m_data.wgtMap.reset(new QLabel);
  m_data.wgtMap->setPixmap(QPixmap("data/config_maze.png"));

  m_data.pages[1]->grid->addWidget(m_data.wgtMap.get(), 0, 1);

  for (int i = 0; i <= 15; ++i) {
    connect(m_data.pages[i].get(), SIGNAL(nextClicked(int)), this, SLOT(onPageNextClick(int)));
    m_data.stackedLayout->addWidget(m_data.pages[i].get());
  }

  m_data.stackedLayout->setCurrentIndex(m_layoutIdxOfFirstConfigPage + 1);
}

//===========================================
// FConfigMaze::onPageNextClick
//===========================================
void FConfigMaze::onPageNextClick(int pageIdx) {
  DBG_PRINT("Showing page " << pageIdx << "\n");

  if (pageIdx != 100) {
    m_data.pages[pageIdx]->reset();
    m_data.stackedLayout->setCurrentIndex(m_layoutIdxOfFirstConfigPage + pageIdx);
  }
  else {
    m_data.stackedLayout->setCurrentIndex(m_layoutIdxOfConsoleLaunchPage);
  }
}

//===========================================
// FConfigMaze::onAreYouSureFinish
//===========================================
void FConfigMaze::onAreYouSureFinish(bool passed) {
  if (passed) {
    m_data.stackedLayout->setCurrentIndex(m_layoutIdxOfConsolePage);
  }
  else {
    m_data.stackedLayout->setCurrentIndex(m_layoutIdxOfConsoleLaunchPage);
  }
}

//===========================================
// FConfigMaze::onEnterConsoleClick
//===========================================
void FConfigMaze::onEnterConsoleClick() {
  m_data.stackedLayout->setCurrentIndex(1);
  m_data.consoleAreYouSurePage.wgtAreYouSure->restart();
}

//===========================================
// FConfigMaze::onExitConsoleClick
//===========================================
void FConfigMaze::onExitConsoleClick() {
  m_data.stackedLayout->setCurrentIndex(0);
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
