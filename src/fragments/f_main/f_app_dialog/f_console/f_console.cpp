#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_console/f_console.hpp"
#include "fragments/f_main/f_app_dialog/f_console/f_console_spec.hpp"
#include "utils.hpp"


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

  const auto& spec = dynamic_cast<const FConsoleSpec&>(spec_);
  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  delete m_data.vbox.release();

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  setLayout(m_data.vbox.get());

  m_data.wgtTextBrowser = makeQtObjPtr<QTextBrowser>(this);
  m_data.vbox->addWidget(m_data.wgtTextBrowser.get());

  m_data.wgtTextBrowser->setText(spec.content);
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
