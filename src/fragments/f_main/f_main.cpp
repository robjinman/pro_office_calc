#include <QMenuBar>
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
    m_data(eventSystem, updateLoop) {}

//===========================================
// FMain::rebuild
//===========================================
void FMain::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FMainSpec&>(spec_);

  setFixedSize(spec.width, spec.height);
  setWindowTitle("Pro Office Calculator");

  m_data.mnuFile.reset(menuBar()->addMenu("File"));
  m_data.actQuit.reset(new QAction("Quit", this));
  m_data.mnuFile->addAction(m_data.actQuit.get());

  connect(m_data.actQuit.get(), SIGNAL(triggered()), this, SLOT(close()));

  m_data.mnuFile->setTitle(spec.fileLabel);
  m_data.actQuit->setText(spec.quitLabel);

  setColour(*this, spec.bgColour, QPalette::Window);
  setWindowTitle(spec.windowTitle);

  Fragment::rebuild(spec_);
}

//===========================================
// FMain::closeEvent
//===========================================
void FMain::closeEvent(QCloseEvent*) {
  m_data.fnOnQuit();
  DBG_PRINT("Quitting\n");
}

//===========================================
// FMain::cleanUp
//===========================================
void FMain::cleanUp() {

}

//===========================================
// FMain::~FMain
//===========================================
FMain::~FMain() {
  DBG_PRINT("FMain::~FMain()\n");
}
