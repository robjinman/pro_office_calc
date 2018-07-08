#include <string>
#include <vector>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_console/f_console.hpp"
#include "fragments/f_main/f_app_dialog/f_console/f_console_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/events.hpp"
#include "utils.hpp"


using std::string;
using std::vector;

typedef ConsoleWidget::ArgList ArgList;


//===========================================
// FConsole::FConsole
//===========================================
FConsole::FConsole(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FConsole", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FConsole::FConsole\n");
}

//===========================================
// FConsole::reload
//===========================================
void FConsole::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FConsole::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  delete m_data.vbox.release();

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  setLayout(m_data.vbox.get());

  string initialContent =
    "┌───────────────────────────────────────┐\n"
    "│ Apex OS Terminal                      │\n"
    "├───────────────────────────────────────┤\n"
    "│ Host name        zpx11                │\n"
    "│ Date and time    1998/12/14 14:08:15  │\n"
    "│ Logged in as     susan                │\n"
    "│ Logged in since  1998/12/14 11:50:06  │\n"
    "└───────────────────────────────────────┘\n"
    "> ";

  m_commandsEntered = 0;

  m_data.wgtConsole = makeQtObjPtr<ConsoleWidget>(initialContent, vector<string>{});
  m_data.vbox->addWidget(m_data.wgtConsole.get());

  m_hCommandsGenerated = commonData.eventSystem.listen("doomsweeper/commandsGenerated",
    [this](const Event& e_) {

    auto& e = dynamic_cast<const doomsweeper::CommandsGeneratedEvent&>(e_);
    addCommands(e.commands);
  });
}

//===========================================
// FConsole::addCommands
//===========================================
void FConsole::addCommands(const vector<vector<string>>& commands) {
  int numCommands = commands.size();

  for (int i = 0; i < numCommands; ++i) {
    auto& cmd = commands[i];

    const string& prog = cmd.front();
    vector<string> parts{cmd.begin() + 1, cmd.end()};

    m_data.wgtConsole->addCommand(prog, [this, parts, i, numCommands](const ArgList& args) {
      if (args == parts && m_commandsEntered == i) {
        m_commandsEntered = i + 1;

        if (m_commandsEntered == numCommands) {
          commonData.eventSystem.fire(pEvent_t(new Event{"doomsweeper/commandsEntered"}));
        }

        return "Success";
      }

      return "Failed";
    });
  }
}

//===========================================
// FConsole::cleanUp
//===========================================
void FConsole::cleanUp() {
  DBG_PRINT("FConsole::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FConsole::~FConsole
//===========================================
FConsole::~FConsole() {
  DBG_PRINT("FConsole::~FConsole\n");
}
