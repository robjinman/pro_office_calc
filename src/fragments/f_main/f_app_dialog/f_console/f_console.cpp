#include <string>
#include <vector>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_console/f_console.hpp"
#include "fragments/f_main/f_app_dialog/f_console/f_console_spec.hpp"
#include "event_system.hpp"
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
  m_data.wgtConsole->addCommand("hconf", [this](const ArgList& args) {
    if (args == ArgList{"update", "-kbr", "auto=false"}) {
      m_commandsEntered = 1;
      return "config updated";
    }

    return "error updating config";
  });
  m_data.wgtConsole->addCommand("dopler", [this](const ArgList& args) {
    if (args == ArgList{"insert-all", "-g", "bin/extra"}
      && m_commandsEntered == 1) {

      m_commandsEntered = 2;
      return "OK";
    }

    return "FAILED";
  });
  m_data.wgtConsole->addCommand("psched", [this](const ArgList& args) {
    if (args == ArgList{"ccbuild", "+772"} && m_commandsEntered == 2) {
      m_commandsEntered = 3;
      return "action id #7119082 *ccbuild* started";
    }

    return "bad parameters";
  });
  m_data.wgtConsole->addCommand("xiff", [this](const ArgList& args) {
    if (args == ArgList{"16:2", "--single-pass", "--retry", "10"}
      && m_commandsEntered == 3) {

      commonData.eventSystem.fire(pEvent_t(new Event{"doomsweeper/commandsEntered"}));

      return "[status a51e] finished";
    }

    return "invalid options";
  });

  m_data.vbox->addWidget(m_data.wgtConsole.get());
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
