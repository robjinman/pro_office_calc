#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_main_spec.hpp"
#include "utils.hpp"
#include "exception.hpp"
#include "effects.hpp"


using std::string;
using std::unique_ptr;


//===========================================
// FMain::FMain
//===========================================
FMain::FMain(EventSystem& eventSystem, UpdateLoop& updateLoop)
  : QMainWindow(nullptr),
    Fragment("FMain", m_data),
    m_data(eventSystem, updateLoop) {

  setFixedSize(300, 260);
  setWindowTitle("Pro Office Calculator");
}

//===========================================
// FMain::rebuild
//===========================================
void FMain::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FMainSpec&>(spec_);

  setColour(*this, spec.bgColour, QPalette::Window);
  setWindowTitle(spec.windowTitle);

  Fragment::rebuild(spec_);
}

//===========================================
// FMain::cleanUp
//===========================================
void FMain::cleanUp() {

}

//===========================================
// FMain::closeEvent
//===========================================
void FMain::closeEvent(QCloseEvent*) {
  m_data.fnOnQuit();
  DBG_PRINT("Quitting\n");
}

//===========================================
// FMain::~FMain
//===========================================
FMain::~FMain() {
  DBG_PRINT("FMain::~FMain()\n");
}
